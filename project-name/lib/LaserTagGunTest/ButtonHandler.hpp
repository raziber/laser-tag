#pragma once

#include <Arduino.h>

class ButtonHandler {
public:
    ButtonHandler(gpio_num_t buttonPin);
    ~ButtonHandler();

    esp_err_t initialize();
    void monitor(); // Called within the task loop

    bool isButtonPressed();

private:
    gpio_num_t buttonPin_;
    bool lastButtonState_;
    bool buttonPressed_;
    unsigned long lastDebounceTime_;
    const unsigned long debounceDelay_;

    SemaphoreHandle_t buttonMutex_;
};
