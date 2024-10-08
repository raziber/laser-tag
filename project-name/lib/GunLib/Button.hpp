// Button.hpp
#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "Queue.hpp"
#include <memory>
#include "Task.hpp"

class Button {
public:
    Button(gpio_num_t pin, TickType_t debounceTime);
    void startMonitoring(Queue<bool>& buttonQueue);
    ~Button();

private:
    void buttonTaskFunction(Queue<bool>& buttonQueue);
    std::unique_ptr<Task> buttonTask_;  // Use the Task class instead of raw task handle
    gpio_num_t pin_;  // GPIO pin for the button
    TickType_t debounceTime_;  // Debounce time to prevent false triggers

    SemaphoreHandle_t buttonMutex_;
};
