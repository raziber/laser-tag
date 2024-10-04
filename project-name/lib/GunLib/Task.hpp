// Task.hpp
#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <functional>
#include <string>

class Task {
public:
    Task(const std::string& name, uint32_t stackSize, UBaseType_t priority, const std::function<void()>& taskFunction);
    ~Task();

private:
    static void taskEntryPoint(void* params);
    TaskHandle_t handle_;  // Handle for the FreeRTOS task
    std::function<void()> taskFunction_;  // Function to execute in the task
};
