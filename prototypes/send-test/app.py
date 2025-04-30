import socket, wave, time, sys

ESP_IP  = "192.168.2.101"     # ESP32 の IP
PORT    = 5005
FILE    = "converted.wav"   # 8kHz / 8bit / mono PCM

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

with wave.open(FILE, 'rb') as w:
    assert w.getframerate()==8000 and w.getnchannels()==1 and w.getsampwidth()==1
    CHUNK = 512
    data = w.readframes(CHUNK)
    while data:
        sock.sendto(data, (ESP_IP, PORT))
        time.sleep(CHUNK / 8000.0)   # 実時間に合わせる
        data = w.readframes(CHUNK)

sock.close()
print("done")
