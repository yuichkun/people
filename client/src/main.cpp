#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_timer.h>

// ─── 設定 ───
const char* SSID = "escentier";
const char* PASS = "escentier";

constexpr int   SR        = 8000;        // 8 kHz
constexpr uint8_t DAC_PIN = 25;          // GPIO25 内蔵 DAC1
constexpr size_t RING     = 16384;       // 16 kB (≈ 2 s)
constexpr int   UDP_PORT  = 5005;        // 任意

// ─── バッファ ───
uint8_t ring[RING];
volatile size_t head = 0, tail = 0;      // write / read index

inline size_t freeBytes() { return (tail + RING - head - 1) % RING; }
inline size_t usedBytes() { return (head + RING - tail) % RING;     }

// ─── UDP ───
WiFiUDP udp;

// ─── 再生 ISR (8 kHz) ───
void IRAM_ATTR onAudio(void*) {
  if (usedBytes() == 0) {                // バッファ空 → 無音 0x80
    dacWrite(DAC_PIN, 0x80);
    return;
  }
  uint8_t s = ring[tail];
  tail = (tail + 1) % RING;
  dacWrite(DAC_PIN, s);
}

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

void setup() {
  Serial.begin(115200);

  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\nIP=" + WiFi.localIP().toString());

  // UDP 待ち受け
  udp.begin(UDP_PORT);
  Serial.printf("UDP port %d open\n", UDP_PORT);

  // 受信タスクを Core0 (Wi-Fi と同コア) に
  xTaskCreatePinnedToCore(udpTask, "udpTask", 4096, nullptr, 1, nullptr, 0);

  // 再生タイマー (Core1 で実行)
  esp_timer_create_args_t cfg{
    .callback = &onAudio,
    .arg      = nullptr,
    .name     = "audio"
  };
  esp_timer_handle_t t;
  esp_timer_create(&cfg, &t);
  esp_timer_start_periodic(t, 1000000 / SR);   // µs
}

void loop() {}
