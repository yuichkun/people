#include "OLEDManager.h"

OLEDManager::OLEDManager() 
    : u8g2(U8G2_R0, U8X8_PIN_NONE), 
      text("Hello World"),
      textX(128),
      lastUpdateTime(0),
      updateInterval(50) // 50ms update interval like in the example
{
}

OLEDManager::~OLEDManager() {
}

bool OLEDManager::begin(uint8_t sda, uint8_t scl) {
    Wire.begin(sda, scl);
    if (!u8g2.begin()) {
        return false;
    }
    u8g2.enableUTF8Print();
    return true;
}

void OLEDManager::setText(const char* newText) {
    text = newText;
    textX = 128; // Reset position when text changes
}

void OLEDManager::update() {
    if (millis() - lastUpdateTime >= updateInterval) {
        lastUpdateTime = millis();
        
        u8g2.setFont(u8g2_font_unifont_t_japanese3);
        int w = u8g2.getUTF8Width(text);
        
        if (w <= 128) {
            textX = 0; // If text fits on screen, don't scroll
        }
        
        u8g2.clearBuffer();
        u8g2.setCursor(textX, 32); // Y=32 centers text vertically
        u8g2.print(text);
        u8g2.sendBuffer();
        
        if (w > 128) { // Scroll only if text is wider than screen
            if (--textX < -w) textX = 128;
        }
    }
} 