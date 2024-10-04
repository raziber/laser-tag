#pragma once

#include "MFRC522.hpp"
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <memory>

class RFIDReader {
public:
    RFIDReader(std::unique_ptr<MFRC522> mfrc522);
    ~RFIDReader();

    esp_err_t initialize();
    void scan();
    std::string getLastTagId();

private:
    esp_err_t initHardware();

    std::unique_ptr<MFRC522> mfrc522_;
    std::string lastTagId_;
    SemaphoreHandle_t tagMutex_;
};
