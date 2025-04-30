#include <WiFi.h>
#include <WebServer.h>
#include <esp_timer.h>   // 高分解能タイマー (µs)

// ─── Wi-Fi ───
const char* WIFI_SSID = "escentier";
const char* WIFI_PASS = "escentier";

// ─── Audio ───
constexpr uint32_t SAMPLE_RATE = 8000;     // 8 kHz
constexpr float    DEFAULT_FREQ = 220.0f;  // 起動時 220 Hz
const uint8_t      DAC_PIN = 25;           // **GPIO25 (必ず 25 or 26)**

// ─── Globals ───
WebServer server(80);
uint8_t  sineLUT[256];
volatile uint32_t phaseAcc = 0, phaseInc = 0;
esp_timer_handle_t audioTimer;

// ─── Timer ISR ───
void IRAM_ATTR onAudio(void*) {
  phaseAcc += phaseInc;
  uint8_t idx = phaseAcc >> 24;      // 上位 8bit
  dacWrite(DAC_PIN, sineLUT[idx]);   // **25 → DAC1**
}

// ─── HTTP /freq?f=xxx ───
void handleFreq() {
  if (!server.hasArg("f")) { server.send(400,"text/plain","?f=Hz"); return; }
  float f = server.arg("f").toFloat();
  if (f <= 0)          { server.send(400,"text/plain","bad"); return; }
  phaseInc = uint32_t((f * (1ULL<<32)) / SAMPLE_RATE);
  server.send(200,"text/plain","Set "+String(f)+" Hz");
}

void setup() {
  Serial.begin(115200);

  // 1) Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi");
  while (WiFi.status()!=WL_CONNECTED){delay(300);Serial.print(".");}
  Serial.println("\nIP="+WiFi.localIP().toString());

  // 2) 256-pt sine LUT (0-255)
  for (int i=0;i<256;i++){
    float th = 2*PI*i/256.0f;
    sineLUT[i] = uint8_t(127.5f + 127.5f * sinf(th));
  }

  // 3) 初期周波数
  phaseInc = uint32_t((DEFAULT_FREQ * (1ULL<<32)) / SAMPLE_RATE);

  // 4) DAC 初期化（中央値）
  dacWrite(DAC_PIN, 128);

  // 5) esp_timer 8 kHz
  esp_timer_create_args_t cfg{
    .callback = &onAudio,
    .arg      = nullptr,
    .dispatch_method = ESP_TIMER_TASK, // タスクコンテキスト
    .name     = "audio"
  };
  esp_timer_create(&cfg, &audioTimer);
  esp_timer_start_periodic(audioTimer, 1000000 / SAMPLE_RATE); // µs

  // 6) HTTP
  server.on("/freq", HTTP_GET, handleFreq);
  server.begin();
}

void loop(){ server.handleClient(); }
