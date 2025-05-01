#include <WiFi.h>
#include <WiFiUdp.h>
#include "config.h"
#include "buffer.h"
#include "AudioManager.h"
#include "OLEDManager.h"

// ─── UDP ───
WiFiUDP udp;
AudioManager audioManager;
OLEDManager oledManager;

// ─── 受信タスク (Core0) ───
void udpTask(void*) {
  uint8_t buf[512];
  for (;;) {
    int len = udp.parsePacket();
    if (len <= 0) { vTaskDelay(1); continue; }

    len = udp.read(buf, sizeof(buf));
    for (int i = 0; i < len; ++i) {
      if (freeBytes() == 0) {            // 溢れたら古いデータ捨てる
        tail = (tail + 1) % RING;
      }
      ring[head] = buf[i];
      head = (head + 1) % RING;
    }
  }
}

// ─── ディスプレイタスク (Core1) ───
void displayTask(void*) {
  for (;;) {
    oledManager.update();
    vTaskDelay(1); // Short delay to prevent watchdog reset
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nIP=" + WiFi.localIP().toString());

  // OLED ディスプレイ初期化
  if (!oledManager.begin(21, 22)) {
    Serial.println("Failed to start OLED display");
  } else {
    Serial.println("OLED display started");
    oledManager.setText("うお〜〜！コップにはめて見るとイカちぃ〜最高！！");
  }

  // UDP 待ち受け
  udp.begin(UDP_PORT);
  Serial.printf("UDP port %d open\n", UDP_PORT);

  // 受信タスクを Core0 (Wi-Fi と同コア) に
  xTaskCreatePinnedToCore(udpTask, "udpTask", 4096, nullptr, 1, nullptr, 0);
  
  // ディスプレイタスクを Core1 に
  xTaskCreatePinnedToCore(displayTask, "displayTask", 4096, nullptr, 1, nullptr, 1);

  // 再生タイマー設定
  if (!audioManager.begin()) {
    Serial.println("Failed to start audio manager");
  } else {
    Serial.println("Audio manager started");
  }
}

void loop() {}
