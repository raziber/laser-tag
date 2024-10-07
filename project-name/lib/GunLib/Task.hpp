// Task.hpp
#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <functional>
#include <string>
#include <atomic>

class Task {
public:
    Task(const std::string& name, uint32_t stackSize, UBaseType_t priority, const std::function<void()>& taskFunction);
    ~Task();

    // Provide a way to signal the task to stop
    void requestStop();
    bool shouldStop() const;

private:
    TaskHandle_t handle_;  // Handle for the FreeRTOS task
    std::function<void()> taskFunction_;  // Function to execute in the task
    std::atomic<bool> shouldStop_;  // Flag to signal the task to stop

    static void taskEntryPoint(void* params);
};