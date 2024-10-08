// MutexLockGuard.cpp
#include "MutexLockGuard.hpp"
#include "esp_log.h"
#include <stdexcept>

MutexLockGuard::MutexLockGuard(SemaphoreHandle_t mutex, TickType_t timeout)
    : mutex_(mutex), isLocked_(false) {
    if (mutex_ == nullptr) {
        throw std::runtime_error("Mutex handle is null");
    }

    if (xSemaphoreTakeRecursive(mutex_, timeout) != pdTRUE) {
        throw std::runtime_error("Failed to acquire mutex");
    }
    isLocked_ = true;
}

MutexLockGuard::~MutexLockGuard() {
    if (isLocked_) {
        xSemaphoreGiveRecursive(mutex_);
    }
}

MutexLockGuard::MutexLockGuard(MutexLockGuard&& other) noexcept
    : mutex_(other.mutex_), isLocked_(other.isLocked_) {
    other.mutex_ = nullptr;
    other.isLocked_ = false;
}

MutexLockGuard& MutexLockGuard::operator=(MutexLockGuard&& other) noexcept {
    if (this != &other) {
        if (isLocked_) {
            xSemaphoreGiveRecursive(mutex_);
        }
        mutex_ = other.mutex_;
        isLocked_ = other.isLocked_;
        other.mutex_ = nullptr;
        other.isLocked_ = false;
    }
    return *this;
}

bool MutexLockGuard::isLocked() const {
    return isLocked_;
}
