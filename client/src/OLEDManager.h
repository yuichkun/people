#ifndef OLED_MANAGER_H
#define OLED_MANAGER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

class OLEDManager {
private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C* u8g2;
    String currentText;
    int textX;
    unsigned long lastUpdateTime;
    int updateInterval;
    uint8_t sdaPin;
    uint8_t sclPin;
    bool verticalFlip;
    
public:
    OLEDManager();
    ~OLEDManager();
    
    bool begin(uint8_t sda = 21, uint8_t scl = 22);
    void setText(const String& newText);
    void update();
    void setVerticalFlip(bool flip);
    void reinitialize();
};

#endif // OLED_MANAGER_H 