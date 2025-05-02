from nlp import modifySentence
from network import sendText
import logging
import time


def main():
    # Set up logging
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s [%(levelname)s] %(message)s'
    )

    # Hardcoded list of client IP addresses (in order)
    client_ip_addresses = [
        # 1
        "192.168.2.103",
        # 2
        "192.168.2.104",
        # 3
        "192.168.2.105",
        # 4
        "192.168.2.106",
        # 5
        "192.168.2.107",
        # 6
        "192.168.2.100",
        # 7
        "192.168.2.108",
        # 8
        "192.168.2.109",
        # 9
        "192.168.2.110",
    ]

    # Hardcoded list of original sentences
    original_sentences = [
        "今日は良い天気です。",
        "私は猫が好きです。",
        "東京は大きな都市です。",
        # Add more sentences as needed
    ]

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
                    time.sleep(1)  # Optional: avoid tight infinite loop
                except Exception as e:
                    logging.error(
                        f"Error processing client {client_ip} with sentence: "
                        f"'{last_sentence}'. Error: {e}"
                    )
                    continue
            time.sleep(4)  # Optional: avoid tight infinite loop


if __name__ == "__main__":
    main()
