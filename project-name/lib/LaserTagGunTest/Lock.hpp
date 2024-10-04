/**
 * @brief Takes a mutex and releases it when out of scope
 * 
 * use by doing this:
 * Lock lock = Lock::make(specificMutex);
 */

#pragma once

#include <optional>
#include <Arduino.h>

class Lock{
public:
    static std::optional<Lock> make(SemaphoreHandle_t mutex);

    ~Lock();
private:
    SemaphoreHandle_t mutex_;

    Lock(SemaphoreHandle_t mutex);
};
