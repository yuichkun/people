from nlp import modifySentence
from network import sendText
import logging
import time
import random


def main():
    # Set up logging
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s [%(levelname)s] %(message)s'
    )

    # Hardcoded list of client IP addresses (in order)
    client_ip_addresses = [
        # 1
        "192.168.2.102",
        # 2
        "192.168.2.103",
        # 3
        "192.168.2.104",
        # 4
        "192.168.2.105",
        # 5
        "192.168.2.106",
        # 6
        "192.168.2.107",
        # 7
        "192.168.2.108",
        # 8
        # "192.168.2.109",
        # # 9
        # "192.168.2.101",
    ]

    # Hardcoded list of original sentences
    original_sentences = [
        "寝る前に良い実装思いついてしまった",
        "鬱で浴槽から出られず無駄な時間を使ってしまった",
        "代々木公園の大石良すぎる",
        "たぬき顔すき",
        "マツケンサンバいったいいつまで擦られるんだろう",
        "何も予約してないけど今から福岡行こ",
        "自分の作品をアイコンにするのが夢",
        "問題はウニをダイエット後か先か",
        "全然仕事進まんな。休憩。",
        "GWあんま寝れてないな",
        "もうしばらくゲームは買わない",
        "この髪型も好き",
        "おやつはカツカレー",
        "5月もたっぷり楽しむぞー",
        "いい気候でのロケ　すばらしい",
        "ホント大迷惑。",
        "これは鳥肌級に感動した",
        "えっ！？これ僕だけじゃないの？",
        "あんまりにもしつこいから非表示にしたわ",
        "展示の空間案が出来上がりつつある。",
        "一周回って「プロンプト大事だ」という気持ちになってきた。",
        "楽天証券、2段階認証がなんの意味もないｗ",
    ]
    random.shuffle(original_sentences)

    # Infinite loop
    while True:
        for idx, original_sentence in enumerate(original_sentences):
            last_sentence = original_sentence
            logging.info(
                f"Processing sentence {idx+1}/{len(original_sentences)}: "
                f"'{original_sentence}'"
            )

            for i, client_ip in enumerate(client_ip_addresses):
                try:
                    if i == 0:
                        modified_sentence = last_sentence
                        logging.info(
                            f"[Client {i+1}/{len(client_ip_addresses)}] "
                            f"Sending original sentence to {client_ip}: "
                            f"'{modified_sentence}'"
                        )
                    else:
                        logging.info(
                            f"[Client {i+1}/{len(client_ip_addresses)}] "
                            f"Modifying sentence for {client_ip}..."
                        )
                        modified_sentence = modifySentence(last_sentence, client_ip)
                        logging.info(
                            f"[Client {i+1}/{len(client_ip_addresses)}] "
                            f"Modified sentence: '{modified_sentence}'"
                        )

                    sendText(modified_sentence, client_ip)
                    logging.info(
                        f"[Client {i+1}/{len(client_ip_addresses)}] "
                        f"Sent sentence to {client_ip}"
                    )
                    last_sentence = modified_sentence
                    time.sleep(4)  # Optional: avoid tight infinite loop
                except Exception as e:
                    logging.error(
                        f"Error processing client {client_ip} with sentence: "
                        f"'{last_sentence}'. Error: {e}"
                    )
                    continue
            time.sleep(4)  # Optional: avoid tight infinite loop


if __name__ == "__main__":
    main()
