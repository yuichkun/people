#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <Arduino.h>
#include <esp_timer.h>
#include "config.h"
#include "buffer.h"

class AudioManager {
private:
    esp_timer_handle_t timer;
    static void IRAM_ATTR onAudio(void* arg);
    
public:
    AudioManager();
    ~AudioManager();
    
    bool begin();
    void stop();
};

#endif // AUDIO_MANAGER_H 