import vibrato
import zstandard
dctx = zstandard.ZstdDecompressor()
with open('ipadic-mecab-2_7_0/system.dic.zst', 'rb') as fp:
    with dctx.stream_reader(fp) as dict_reader:
        tokenizer = vibrato.Vibrato(dict_reader.read())
        tokens = tokenizer.tokenize("東京スカイツリーへ")
        for token in tokens:
            print(token.surface(), token.feature())