#pragma once

#include <esp_err.h>

class SPIBusManager {
public:
    static esp_err_t initializeBus();
};