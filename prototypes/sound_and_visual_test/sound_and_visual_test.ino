/**
 * ESP32 + PAM8403 + SSD1306
 *  - GPIO25 … DAC_CH1 → PAM8403
 *  - SDA=GPIO16 / SCL=GPIO17 → OLED
 * 
 * 220 Hz サイン波を 16 kHz で割り込み生成（Ticker）
 * OLED は 50 ms 周期で横スクロール
 */
#include <driver/dac.h>
#include <Ticker.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <math.h>

/* ===== Audio parameters ===== */
constexpr uint32_t SAMPLE_RATE     = 16000;   // Hz
constexpr float    FIXED_FREQUENCY = 220.0f;  // Hz
constexpr float    PHASE_INC       =
        2.0f * M_PI * FIXED_FREQUENCY / SAMPLE_RATE;

/* ===== OLED parameters ===== */
U8G2_SSD1306_128X64_NONAME_F_HW_I2C
  u8g2(U8G2_R0, /* reset = */ U8X8_PIN_NONE);

const char *TEXT =
  "うお〜〜！コップにはめて見るとイカちぃ〜最高！！";

/* ===== Globals ===== */
volatile float phase = 0.0f;   // 位相（ISR で更新）
Ticker  audioTicker;

/* ---------- 220 Hz サンプル生成 (ISR) ---------- */
void IRAM_ATTR audioISR()
{
  // サイン波
  float s = sinf(phase);
  uint8_t v = static_cast<uint8_t>(120.0f + (s + 1.0f) * 5.0f); // ±5 LSB

  dac_output_voltage(DAC_CHANNEL_1, v);

  phase += PHASE_INC;
  if (phase >= 2.0f * M_PI) phase -= 2.0f * M_PI;
}

/* ================== SETUP ================== */
void setup()
{
  // --- DAC 初期化 ---
  dac_output_enable(DAC_CHANNEL_1);

  // --- Audio ticker 16 kHz ---
  audioTicker.attach_us(1'000'000UL / SAMPLE_RATE, audioISR);

  // --- OLED 初期化 ---
  Wire.begin(32, 33);              // SDA / SCL
  u8g2.begin();
  u8g2.enableUTF8Print();          // UTF-8 文字列を直接出力可
}

/* ================== LOOP =================== */
void loop()
{
  static int  x = 128;             // テキスト X 位置
  static unsigned long tPrev = 0;

  if (millis() - tPrev >= 50) {    // 50 ms 周期
    tPrev = millis();

    u8g2.setFont(u8g2_font_unifont_t_japanese3);
    int w = u8g2.getUTF8Width(TEXT);

    if (w <= 128) x = 0;           // 画面内に収まるなら固定表示

    u8g2.clearBuffer();
    u8g2.setCursor(x, 32);         // Y=32 で中央寄せ
    u8g2.print(TEXT);
    u8g2.sendBuffer();

    if (w > 128) {                 // はみ出すならスクロール
      if (--x < -w) x = 128;
    }
  }
}
