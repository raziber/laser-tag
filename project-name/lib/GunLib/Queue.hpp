// Queue.hpp
#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdexcept>

template <typename T>
class Queue {
public:
    Queue(UBaseType_t length) {
        // Create a FreeRTOS queue
        queueHandle_ = xQueueCreate(length, sizeof(T));
        if (queueHandle_ == nullptr) {
            throw std::runtime_error("Failed to create queue");
        }
    }

    ~Queue() {
        if (queueHandle_ != nullptr) {
            vQueueDelete(queueHandle_);  // Delete the queue
        }
    }

    bool send(const T& item, TickType_t ticksToWait = portMAX_DELAY) {
        // Send an item to the queue
        return xQueueSend(queueHandle_, &item, ticksToWait) == pdTRUE;
    }

    bool receive(T& buffer, TickType_t ticksToWait = portMAX_DELAY) {
        // Receive an item from the queue
        return xQueueReceive(queueHandle_, &buffer, ticksToWait) == pdTRUE;
    }

private:
    QueueHandle_t queueHandle_;  // Handle for the FreeRTOS queue
};
