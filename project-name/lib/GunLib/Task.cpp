// Task.cpp
#include "Task.hpp"
#include "esp_log.h"

Task::Task(const std::string& name, uint32_t stackSize, UBaseType_t priority, const std::function<void()>& taskFunction)
    : handle_(nullptr), taskFunction_(taskFunction), shouldStop_(false) {
    // Create a FreeRTOS task
    BaseType_t result = xTaskCreate(taskEntryPoint, name.c_str(), stackSize, this, priority, &handle_);
    if (result != pdPASS) {
        ESP_LOGE("Task", "Failed to create task: %s, Stack Size: %u, Priority: %u", name.c_str(), stackSize, priority);
        throw std::runtime_error("Failed to create task: " + name);
    }
}

Task::~Task() {
    if (handle_ != nullptr) {
        // Signal the task to stop
        requestStop();

        // Optionally notify the task if it's blocked
        // xTaskNotifyGive(handle_);

        // Wait for the task to terminate
        vTaskDelay(pdMS_TO_TICKS(100));  // Adjust the delay as needed
    }
}

void Task::taskEntryPoint(void* params) {
    Task* task = static_cast<Task*>(params);

    // Execute the task function
    task->taskFunction_();

    // Delete the task when it finishes executing
    vTaskDelete(nullptr);  // The task deletes itself
}

void Task::requestStop() {
    shouldStop_ = true;
}

bool Task::shouldStop() const {
    return shouldStop_;
}
