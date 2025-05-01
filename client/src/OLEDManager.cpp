#include "OLEDManager.h"

OLEDManager::OLEDManager() 
    : u8g2(nullptr),
      currentText("Hello World"),
      textX(128),
      lastUpdateTime(0),
      updateInterval(50),
      sdaPin(21),
      sclPin(22)
{
}

OLEDManager::~OLEDManager() {
    if (u8g2 != nullptr) {
        delete u8g2;
        u8g2 = nullptr;
    }
}

bool OLEDManager::begin(uint8_t sda, uint8_t scl) {
    sdaPin = sda;
    sclPin = scl;
    
    Wire.begin(sdaPin, sclPin);
    
    // Create new instance
    reinitialize();
    
    return (u8g2 != nullptr);
}

void OLEDManager::reinitialize() {
    // Delete old instance if it exists
    if (u8g2 != nullptr) {
        delete u8g2;
        u8g2 = nullptr;
    }
    
    // Create new instance
    u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
    
    if (u8g2->begin()) {
        u8g2->enableUTF8Print();
        textX = 128; // Reset position
        lastUpdateTime = 0; // Reset timer
    }
}

void OLEDManager::setText(const String& newText) {
    currentText = newText;
    
    // Recreate the display to completely reset the animation
    reinitialize();
}

void OLEDManager::update() {
    if (u8g2 == nullptr) return;
    
    if (millis() - lastUpdateTime >= updateInterval) {
        lastUpdateTime = millis();
        
        u8g2->setFont(u8g2_font_unifont_t_japanese3);
        int w = u8g2->getUTF8Width(currentText.c_str());
        
        if (w <= 128) {
            textX = 0; // If text fits on screen, don't scroll
        }
        
        u8g2->clearBuffer();
        u8g2->setCursor(textX, 32); // Y=32 centers text vertically
        u8g2->print(currentText);
        u8g2->sendBuffer();
        
        if (w > 128) { // Scroll only if text is wider than screen
            if (--textX < -w) textX = 128;
        }
    }
} 