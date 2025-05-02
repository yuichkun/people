#include "HTTPServer.h"
#include <ArduinoJson.h>

HTTPServer::HTTPServer(uint16_t port) 
    : server(port), 
      port(port), 
      running(false) 
{
}

HTTPServer::~HTTPServer() {
    stop();
}

bool HTTPServer::begin() {
    server.begin();
    running = true;
    return true;
}

void HTTPServer::stop() {
    if (running) {
        server.end();
        running = false;
    }
}

void HTTPServer::onTextChange(std::function<void(const String&)> callback) {
    textChangeCallback = callback;
}

void HTTPServer::handleClient() {
    if (!running) return;

    WiFiClient client = server.available();
    if (!client) return;

    // Wait for client data
    unsigned long timeout = millis() + 1000;
    while (!client.available() && millis() < timeout) {
        delay(1);
    }

    if (!client.available()) {
        client.stop();
        return;
    }

    // Read the first line of the request
    String req = client.readStringUntil('\r');
    client.readStringUntil('\n');

    // Parse request
    String method = req.substring(0, req.indexOf(' '));
    String uri = req.substring(req.indexOf(' ') + 1, req.lastIndexOf(' '));

    // Read headers
    String contentType = "";
    int contentLength = 0;
    
    while (client.available()) {
        String line = client.readStringUntil('\r');
        client.readStringUntil('\n');
        
        if (line.length() == 0) break; // End of headers
        
        if (line.startsWith("Content-Type:")) {
            contentType = line.substring(14);
        } else if (line.startsWith("Content-Length:")) {
            contentLength = line.substring(16).toInt();
        }
    }

    // Handle POST request to modify vector
    if (method == "POST" && (uri == "/" || uri == "/vector")) {
        String body = "";
        if (contentLength > 0) {
            char buffer[contentLength + 1];
            client.readBytes(buffer, contentLength);
            buffer[contentLength] = '\0';
            body = String(buffer);
        }

        // Parse JSON request
        StaticJsonDocument<1024> requestDoc;
        DeserializationError error = deserializeJson(requestDoc, body);
        if (error) {
            client.println("HTTP/1.1 400 Bad Request");
            client.println("Content-Type: text/plain");
            client.println("Connection: close");
            client.println();
            client.println("Invalid JSON");
            client.stop();
            return;
        }

        // Generate a random scalar in either [-1.5, -1.0] or [1.0, 1.5]
        float scalar;
        if (random(0, 2) == 0) {
            // Negative range: [-1.5, -1.0]
            scalar = -1.5f + ((float)random(0, 501)) / 1000.0f;
        } else {
            // Positive range: [1.0, 1.5]
            scalar = 1.0f + ((float)random(0, 501)) / 1000.0f;
        }

        // Create response document
        StaticJsonDocument<128> responseDoc;
        responseDoc["scalar"] = scalar;

        // Prepare response
        String response;
        serializeJson(responseDoc, response);

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(response);
        client.stop();
        return;
    }

    // Handle POST request to update text
    if (method == "POST" && uri == "/text") {
        String newText = "";
        if (contentLength > 0) {
            char buffer[contentLength + 1];
            client.readBytes(buffer, contentLength);
            buffer[contentLength] = '\0';
            newText = String(buffer);
        }
        
        if (textChangeCallback && newText.length() > 0) {
            textChangeCallback(newText);
        }
        
        // Send response
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("Text updated");
    } 
    // Basic API info for GET requests
    else if (method == "GET" && uri == "/") {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("API Usage:");
        client.println("POST /text - Update display text");
    } 
    // 404 Not Found
    else {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Connection: close");
        client.println();
    }
    
    // Close the connection
    client.stop();
} 