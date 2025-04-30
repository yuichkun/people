#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <cstddef>

// WiFi configuration
constexpr const char* SSID = "escentier";
constexpr const char* PASS = "escentier";

// Audio configuration
constexpr int   SR        = 8000;        // 8 kHz
constexpr uint8_t DAC_PIN = 25;          // GPIO25 内蔵 DAC1

// Buffer configuration
constexpr size_t RING     = 16384;       // 16 kB (≈ 2 s)

// Network configuration
constexpr int   UDP_PORT  = 5005;        // 任意

#endif // CONFIG_H 