from nlp import find_closest_word
from gensim.models import KeyedVectors

model_path = "word2vec_models/chive-1.3-mc5_gensim/chive-1.3-mc5.kv"
model = KeyedVectors.load(model_path)

word = "犬"
if word in model:
    vec = model[word]
    print(f"Original vector for '{word}': {vec[:5]}...")  # Show first 5 dims

    # 2. Add some value to the vector (e.g., add 0.5 to each dimension)
    modified_vec = vec + 1.5

    # 3. Convert the modified vector to the closest word
    closest = find_closest_word(modified_vec)
    print(f"Closest word to modified vector: {closest}")
else:
    print(f"'{word}' is not in the model vocabulary.")