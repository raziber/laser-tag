#pragma once

#include "IRTransmitter.hpp"
#include "IREncoder.hpp"
#include <memory>

class SimpleIRTransmitter {
public:
    SimpleIRTransmitter(gpio_num_t gpioPin, IRProtocol protocol, uint32_t address = 0x00);
    ~SimpleIRTransmitter();

    esp_err_t transmit(uint32_t code);
    void setAddress(uint32_t address);

private:
    IRTransmitter irTransmitter_;
    uint32_t address_;
};
