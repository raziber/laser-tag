// Button.cpp
#include "Button.hpp"
#include "esp_log.h"

Button::Button(gpio_num_t pin, TickType_t debounceTime)
    : pin_(pin), debounceTime_(debounceTime) {
    // Configure the GPIO pin for the button
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;  // Trigger on positive edge (button press)
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << pin);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;  // Enable pull-up resistor
    gpio_config(&io_conf);
}

void Button::startMonitoring(Queue<bool>& buttonQueue) {
    // Start the button monitoring task using Task class
    try {
        buttonTask_ = std::make_unique<Task>("ButtonTask", 2048, 10, [this, &buttonQueue]() { buttonTaskFunction(buttonQueue); });
    } catch (const std::runtime_error& e) {
        ESP_LOGE("Button", "Failed to create button monitoring task: %s", e.what());
    }
}

void Button::buttonTaskFunction(Queue<bool>& buttonQueue) {
    bool previousState = false;  // Track the previous state of the button
    uint32_t stableStateCount = 0;  // Track stable state count for debouncing
    const uint32_t STABLE_THRESHOLD = 5;  // Number of consistent reads to confirm state change

    while (true) {
        bool currentState = gpio_get_level(pin_);  // Read the current state of the button

        // Check if the current state is different from the previous stable state
        if (currentState != previousState) {
            stableStateCount++;
            if (stableStateCount >= STABLE_THRESHOLD) {
                previousState = currentState;
                stableStateCount = 0;

                if (currentState) {  // Button press detected
                    if (!buttonQueue.send(&currentState, portMAX_DELAY)) {
                        ESP_LOGE("Button", "Failed to send button press to queue");
                    }
                }
            }
        } else {
            stableStateCount = 0;  // Reset stable count if state remains consistent
        }

        vTaskDelay(pdMS_TO_TICKS(10));  // Short delay for debounce checking
    }
}

Button::~Button() {
    if (buttonTask_) {
        buttonTask_.reset();  // Delete the button task
    }
}
