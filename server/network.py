import socket
import wave
from speech import generateSpeech
import requests
import logging


def sendText(text: str, targetIp: str) -> None:
    """
    Generate speech from the given text, convert it to wav, and send the audio
    data over UDP to the target IP at port 5005.
    """

    # Send HTTP POST request to /text endpoint with plain text payload
    url = f"http://{targetIp}/text"
    headers = {'Content-Type': 'text/plain'}
    print('sending request to:', targetIp)
    max_retries = 3
    for attempt in range(1, max_retries + 1):
        try:
            requests.post(
                url,
                data=text.encode('utf-8'),
                headers=headers,
                timeout=5
            )
            break
        except Exception as e:
            if attempt == max_retries:
                logging.warning(
                    f"Failed to send POST to {targetIp} after "
                    f"{max_retries} attempts. Error: {e}"
                )
                raise
            else:
                logging.info(
                    f"Retrying POST to {targetIp} (attempt {attempt}/{max_retries}) "
                    f"due to error: {e}"
                )
                import time
                time.sleep(1)

    # 1. Generate speech and convert to tmp.wav
    generateSpeech(text)

    # 2. Read from tmp.wav and send over UDP
    FILE = "tmp.wav"  # 8kHz / 8bit / mono PCM
    PORT = 5005
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    with wave.open(FILE, 'rb') as w:
        assert (
            w.getframerate() == 8000 and
            w.getnchannels() == 1 and
            w.getsampwidth() == 1
        ), (
            "tmp.wav must be 8kHz, mono, 16bit PCM "
            "(sampwidth=2)"
        )
        CHUNK = 512
        data = w.readframes(CHUNK)
        while data:
            sock.sendto(data, (targetIp, PORT))
            # Sleep to match real time
            import time
            time.sleep(CHUNK / 8000.0)
            data = w.readframes(CHUNK)
    sock.close()
