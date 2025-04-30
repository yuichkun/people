#include <Arduino.h>

void setup() {
  // Initialize serial communication at 115200 bits per second
  Serial.begin(115200);
  // Print a startup message
  Serial.println("ESP32 has started!");
}

void loop() {
  // Print a message every second
  Serial.println("Hello from ESP32!");
  delay(1000);
}
