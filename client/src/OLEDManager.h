#ifndef OLED_MANAGER_H
#define OLED_MANAGER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

class OLEDManager {
private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
    const char* text;
    int textX;
    unsigned long lastUpdateTime;
    int updateInterval;
    
public:
    OLEDManager();
    ~OLEDManager();
    
    bool begin(uint8_t sda = 21, uint8_t scl = 22);
    void setText(const char* newText);
    void update();
};

#endif // OLED_MANAGER_H 