import vibrato
import zstandard
from functools import lru_cache
from typing import List

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
