// Task.cpp
#include "Task.hpp"
#include "esp_log.h"

Task::Task(const std::string& name, uint32_t stackSize, UBaseType_t priority, const std::function<void()>& taskFunction)
    : handle_(nullptr), taskFunction_(taskFunction) {
    // Create a FreeRTOS task
    BaseType_t result = xTaskCreate(taskEntryPoint, name.c_str(), stackSize, this, priority, &handle_);
    if (result != pdPASS) {
        ESP_LOGE("Task", "Failed to create task: %s, Stack Size: %u, Priority: %u", name.c_str(), stackSize, priority);
        throw std::runtime_error("Failed to create task: " + name);
    }
}

Task::~Task() {
    try {
        if (handle_ != nullptr) {
            // Notify the task to stop
            xTaskNotifyGive(handle_);
            // Allow the task to clean up gracefully
            vTaskDelay(pdMS_TO_TICKS(100));  // Allow some time for the task to terminate
    
            // Delete the task if it still exists
            if (eTaskGetState(handle_) != eDeleted) {
                vTaskDelete(handle_);
            }
        }
    } catch (const std::exception& e) {
        ESP_LOGE("Task", "Exception caught during task cleanup: %s", e.what());
    }
}

void Task::taskEntryPoint(void* params) {
    Task* task = static_cast<Task*>(params);

    // Infinite loop that checks for notifications
    while (true) {
        // Wait indefinitely for a notification to continue
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000));

        // Execute the task function
        task->taskFunction_();
    }

    // Clean up resources and delete the task
    vTaskDelete(nullptr);  // Delete the task when it finishes executing
}
