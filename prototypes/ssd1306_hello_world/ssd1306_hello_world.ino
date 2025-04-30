#include <U8g2lib.h>
#include <Wire.h>

// SDA=GPIO16, SCL=GPIO17 でI2Cを設定
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// 表示するテキスト（任意に変更可能）
const char *text = "うお〜〜！コップにはめて見るとイカちぃ〜最高！！";

void setup() {
  Serial.begin(115200); // シリアルモニタを有効化
  
  // I2CピンをGPIO16 (SDA) と GPIO17 (SCL) に設定
  Wire.begin(16, 17); // カスタムピンの指定
  
  u8g2.begin();
  u8g2.enableUTF8Print(); // UTF-8対応を有効化
}

void loop() {
  static int x_pos = 128; // テキストのX位置（初期は画面右端）
  u8g2.setFont(u8g2_font_unifont_t_japanese3); // 以前動いたフォント
  int text_width = u8g2.getUTF8Width(text); // テキストのピクセル幅

  // デバッグ用に値をシリアルモニタに出力
  Serial.print("text_width: ");
  Serial.println(text_width);
  Serial.print("x_pos: ");
  Serial.println(x_pos);

  // テキスト幅が128ピクセル以下ならx_posを0に（画面左端から表示）
  if (text_width <= 128) {
    x_pos = 0;
  }

  u8g2.clearBuffer();
  u8g2.setCursor(x_pos, 32); // Y=32で画面中央（フォント高さ16ピクセルを想定）
  u8g2.print(text);
  u8g2.sendBuffer();

  // スクロール処理
  if (text_width > 128) { // 画面幅を超える場合のみスクロール
    x_pos--; // 左に1ピクセル移動
    if (x_pos < -text_width) { // テキストが画面左端を超えたら
      x_pos = 128; // 画面右端に戻る
    }
  }

  delay(1); // スクロール速度（50msでゆっくり）
}