#include "AudioManager.h"

AudioManager::AudioManager() : timer(nullptr) {
}

AudioManager::~AudioManager() {
    stop();
}

// Audio interrupt service routine
void IRAM_ATTR AudioManager::onAudio(void* arg) {
    if (usedBytes() == 0) {                // バッファ空 → 無音 0x80
        dacWrite(DAC_PIN, 0x80);
        return;
    }
    uint8_t s = ring[tail];
    tail = (tail + 1) % RING;
    dacWrite(DAC_PIN, s);
}

bool AudioManager::begin() {
    // 再生タイマー (Core1 で実行)
    esp_timer_create_args_t cfg{
        .callback = &AudioManager::onAudio,
        .arg      = nullptr,
        .name     = "audio"
    };
    
    esp_err_t err = esp_timer_create(&cfg, &timer);
    if (err != ESP_OK) {
        return false;
    }
    
    err = esp_timer_start_periodic(timer, 1000000 / SR);  // µs
    return err == ESP_OK;
}

void AudioManager::stop() {
    if (timer != nullptr) {
        esp_timer_stop(timer);
        esp_timer_delete(timer);
        timer = nullptr;
    }
} 