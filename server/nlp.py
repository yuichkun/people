import vibrato
import zstandard
from functools import lru_cache
from typing import List, Sequence
from gensim.models import KeyedVectors
import requests

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


def convertWords(words: Sequence[str], ip: str) -> list[str | None]:
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
        List of closest words for each input word (or None if not found).
    """
    endpoint = f"http://{ip}/vector"
    results: list[str | None] = []
    for word in words:
        if word not in model:
            results.append(None)
            continue
        vec = model[word]
        print('sending ', word, len(vec), endpoint)
        response = requests.post(endpoint, json={})
        print('res')
        print(response)
        response.raise_for_status()
        scalar = response.json().get("scalar")
        if scalar is None:
            results.append(None)
            continue
        modified_vec = vec + float(scalar)
        closest = find_closest_word(modified_vec)
        results.append(closest)
    return results
