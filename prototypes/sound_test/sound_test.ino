#include <driver/dac.h>
#include <math.h>

// サンプリングレート（Hz）
#define SAMPLE_RATE 16000
// LFOの周波数範囲（Hz）
#define MIN_FREQUENCY 220
#define MAX_FREQUENCY 12000
// LFOの1サイクル時間（秒）
#define LFO_PERIOD 5.0  // 例: 5秒で220Hz→12000Hz→220Hz

void setup() {
  // DACチャネル1（GPIO25）を有効化
  dac_output_enable(DAC_CHANNEL_1);
}

void loop() {
  // 現在の時間をマイクロ秒で取得
  unsigned long currentMicros = micros();
  static unsigned long previousMicros = 0;
  // サンプリング間隔（マイクロ秒）
  unsigned long interval = 1000000 / SAMPLE_RATE;

  if (currentMicros - previousMicros >= interval) {
    previousMicros = currentMicros;

    // 現在の時刻（秒）を取得
    float time = (float)currentMicros / 1000000.0;

    // LFO：三角波で周波数を変化（0～1の範囲で往復）
    float lfoTime = fmod(time, LFO_PERIOD) / LFO_PERIOD; // 0～1に正規化
    float lfoValue;
    if (lfoTime < 0.5) {
      // 上昇（0～0.5→0～1）
      lfoValue = 2.0 * lfoTime;
    } else {
      // 下降（0.5～1→1～0）
      lfoValue = 2.0 * (1.0 - lfoTime);
    }

    // 周波数を線形補間（220Hz～12000Hz）
    float frequency = MIN_FREQUENCY + (MAX_FREQUENCY - MIN_FREQUENCY) * lfoValue;

    // サイン波の位相を計算
    float angle = 2.0 * PI * frequency * time;
    float sineValue = sin(angle);

    // DAC用にスケール（120～130、PAM8403の感度を考慮）
    uint8_t dacValue = (uint8_t)(120 + (sineValue + 1.0) * 5);

    // DACに出力
    dac_output_voltage(DAC_CHANNEL_1, dacValue);
  }
}