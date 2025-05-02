import vibrato
import zstandard
from functools import lru_cache
from typing import List
from gensim.models import KeyedVectors

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

