#include "ButtonHandler.hpp"

// Constructor
ButtonHandler::ButtonHandler(gpio_num_t buttonPin)
    : buttonPin_(buttonPin), lastButtonState_(true), lastDebounceTime_(0),
      debounceDelay_(50), buttonMutex_(nullptr) {
    buttonMutex_ = xSemaphoreCreateMutex();
    if (buttonMutex_ == nullptr) {
        ESP_LOGE("ButtonHandler", "Failed to create button mutex");
    }
}

// Destructor
ButtonHandler::~ButtonHandler() {
    if (buttonMutex_ != nullptr) {
        vSemaphoreDelete(buttonMutex_);
        buttonMutex_ = nullptr;
    }
}

// Initialize button GPIO
esp_err_t ButtonHandler::initialize() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << buttonPin_);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    return gpio_config(&io_conf);
}

// Monitor button state
void ButtonHandler::monitor() {
    bool reading = gpio_get_level(buttonPin_);

    if (reading != lastButtonState_) {
        lastDebounceTime_ = xTaskGetTickCount();
    }

    if ((xTaskGetTickCount() - lastDebounceTime_) > pdMS_TO_TICKS(debounceDelay_)) {
        if (reading == false && lastButtonState_ == true) { // Assuming active low
            // Button pressed
            if (xSemaphoreTake(buttonMutex_, portMAX_DELAY) == pdTRUE) {
                buttonPressed_ = true;
                xSemaphoreGive(buttonMutex_);
            }
        }
    }

    lastButtonState_ = reading;
}

// Check if button was pressed
bool ButtonHandler::isButtonPressed() {
    bool pressed = false;
    if (xSemaphoreTake(buttonMutex_, portMAX_DELAY) == pdTRUE) {
        pressed = buttonPressed_;
        buttonPressed_ = false;
        xSemaphoreGive(buttonMutex_);
    }
    return pressed;
}
