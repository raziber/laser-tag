#include "SimpleIRTransmitter.hpp"

SimpleIRTransmitter::SimpleIRTransmitter(gpio_num_t gpioPin, IRProtocol protocol, uint32_t address)
    : irTransmitter_({static_cast<int>(gpioPin)}, std::make_unique<IREncoder>(protocol), 1, 80),
      address_(address) {
    // The IRTransmitter is initialized with a vector containing one GPIO pin
}

SimpleIRTransmitter::~SimpleIRTransmitter() {
    // Resources are managed by IRTransmitter
}

esp_err_t SimpleIRTransmitter::transmit(uint32_t code) {
    return irTransmitter_.transmitToAllPorts(address_, code);
}

void SimpleIRTransmitter::setAddress(uint32_t address) {
    address_ = address;
}
