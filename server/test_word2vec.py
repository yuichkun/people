from gensim.models import KeyedVectors

# モデルのパスを指定
model_path = "word2vec_models/chive-1.3-mc5_gensim/chive-1.3-mc5.kv"

# モデルをロード
model = KeyedVectors.load(model_path)

# 試しに「犬」のベクトルと類似単語を取得
word = "犬"
if word in model:
    print(f"「{word}」のベクトル（最初の5次元）: {model[word][:5]}")
    print(f"「{word}」に近い単語: {model.most_similar(word, topn=5)}")
else:
    print(f"「{word}」はモデルにありません")