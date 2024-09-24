#pragma once

#include <Arduino.h>
#include "MFRC522.hpp"

class RFIDReader {
public:
    RFIDReader();
    ~RFIDReader();

    esp_err_t initialize();
    void scan(); // Called within the task loop
    std::string getLastTagId();

private:
    // Hardware initialization and utilities
    esp_err_t initHardware();

    // Internal variables
    std::string lastTagId_;
    SemaphoreHandle_t tagMutex_; // Protects lastTagId_

    // MFRC522 instance or equivalent
    MFRC522 mfrc522_;
};
