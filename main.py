from multiprocessing import Pool
from collections import Counter

words = []
#start_word = "arose"

def process_words():
    with open("words.txt", 'r') as f:
        for word in f:
            if word[-1] == "\n":
                word = word[:-1]
            words.append(word)

def process_start_word(start_word):
    word_score = []
    for word in words:
        if word[-1] == "\n":
            exit(1)
        score = []
        seen_b = []
        for char_a, char_b in zip(word, start_word):
            if char_a == char_b:
                score.append("🟩")
            elif char_b in word:
                if char_b in seen_b:
                    if seen_b.count(char_b) < word.count(char_b):
                        score.append("🟨")
                    else:
                        score.append("⬛")
                else:
                    score.append("🟨")
            else:
                score.append("⬛")
            seen_b.append(char_b)
        word_score.append(score)

    counts = Counter()
    for score in word_score:
        counts[tuple(score)] += 1
        
    return start_word, counts

# MAIN
if __name__ == "__main__":

    process_words()

    with Pool(4, initializer=process_words) as p:
        for start_word, counts in p.imap_unordered(process_start_word, ["myrrh"], chunksize=16):#["arose", "bound", "scare", "neigh", "myrrh", "banjo"])
            print(start_word, counts.items())