#pragma once

#include <Arduino.h>
#include "IRProtocolEnum.hpp"

class IRTransmitter {
public:
    IRTransmitter(gpio_num_t irLedPin, IRProtocol protocol);
    ~IRTransmitter();

    esp_err_t initialize();
    esp_err_t transmit(uint32_t code);

private:
    gpio_num_t irLedPin_;
    IRProtocol protocol_;
    // Internal variables for IR encoding
};
