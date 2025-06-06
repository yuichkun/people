import vibrato
import zstandard
from functools import lru_cache
from typing import List, Sequence
from gensim.models import KeyedVectors
import requests
import random

model_path = "word2vec_models/chive-1.3-mc5_gensim/chive-1.3-mc5.kv"
model = KeyedVectors.load(model_path)

# Path to the compressed dictionary
_DIC_PATH = 'ipadic-mecab-2_7_0/system.dic.zst'


@lru_cache(maxsize=1)
def _get_tokenizer():
    dctx = zstandard.ZstdDecompressor()
    with open(_DIC_PATH, 'rb') as fp:
        with dctx.stream_reader(fp) as dict_reader:
            return vibrato.Vibrato(dict_reader.read())


def extract_nouns(text: str) -> List[str]:
    """
    Extracts all nouns from the input Japanese text using the Vibrato
    tokenizer.
    Returns a list of noun surfaces.
    """
    tokenizer = _get_tokenizer()
    tokens = tokenizer.tokenize(text)
    nouns = [
        token.surface() for token in tokens
        if token.feature().startswith('名詞')
    ]
    return nouns


def find_closest_word(vector):
    """
    Given a vector, find and return the closest word in the loaded
    word2vec model.
    """
    # model.most_similar returns a list of (word, similarity) tuples
    closest = model.most_similar([vector], topn=1)
    return closest[0][0] if closest else None


def convertWords(
    words: Sequence[str], ip: str
) -> list[tuple[str, str | None]]:
    """
    For each word in the input list, get its vector, request a random scalar
    from the remote endpoint (constructed from the given IP), add this scalar
    to each element of the vector, and convert the modified vector to the
    closest word. The request body no longer includes the vector, only an
    empty JSON object.
    Args:
        words: List of words to process.
        ip: IP address of the remote server (endpoint will be constructed as
            'http://<ip>/vector').
    Returns:
        List of tuples containing (original_word, replacement_word) for each
        input word. replacement_word can be None if processing failed.
    """
    endpoint = f"http://{ip}/vector"
    results: list[tuple[str, str | None]] = []
    for word in words:
        if word not in model:
            results.append((word, None))
            continue
        vec = model[word]
        print('sending ', word, len(vec), endpoint)
        response = requests.post(endpoint, json={}, timeout=2)
        print('res')
        print(response)
        response.raise_for_status()
        scalar = response.json().get("scalar")
        if scalar is None:
            results.append((word, None))
            continue
        modified_vec = vec + float(scalar)
        closest = find_closest_word(modified_vec)
        results.append((word, closest))
    return results


def modifySentence(sentence: str, remoteIp: str) -> str:
    """
    1. Extract nouns from the sentence using extract_nouns
    2. For the extracted nouns, run convertWords with the remote ip
    3. Replace one randomly selected noun with the response
    4. Return the modified sentence
    """
    # 1. Extract nouns
    nouns = extract_nouns(sentence)

    # If no nouns detected, return original sentence
    if not nouns:
        return sentence

    # 2. Randomly select one noun
    selected_noun = random.choice(nouns)

    # 3. Convert the selected noun using remote IP
    word_pairs = convertWords([selected_noun], remoteIp)

    # 4. Replace the noun in the sentence
    modified = sentence

    # Get the replacement (if not None)
    original, replacement = word_pairs[0]
    if replacement is not None:
        modified = modified.replace(original, replacement)

    return modified
