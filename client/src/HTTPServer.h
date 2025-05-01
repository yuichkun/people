#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <functional>

class HTTPServer {
private:
    WiFiServer server;
    uint16_t port;
    std::function<void(const String&)> textChangeCallback;
    bool running;

public:
    HTTPServer(uint16_t port = 80);
    ~HTTPServer();

    bool begin();
    void stop();
    void handleClient();
    void onTextChange(std::function<void(const String&)> callback);
};

#endif // HTTP_SERVER_H 