#pragma once

#include "IRProtocols/IRProtocolSettings.hpp"
#include "IREncoder.hpp"
#include <driver/rmt.h>
#include <memory>
#include <Arduino.h>

class IRTransmitter{
public:
    IRTransmitter(const std::vector<int>& ports, std::unique_ptr<IREncoder> encoder, std::unique_ptr<IRProtocolSettings> protocolSettings, int memBlockNum, int clkDiv);
    ~IRTransmitter();

    esp_err_t transmitToAllPorts(uint32_t address, uint32_t command) const;
    esp_err_t transmitToSinglePort(uint32_t address, uint32_t command, uint32_t portId) const;
private:
    std::unique_ptr<IREncoder> encoder_;
    std::unique_ptr<IRProtocolSettings> protocolSettings_;
    std::vector<int> gpioPorts_;  // A list of ports (RMT channels or GPIO pins)
    int memBlockNum_;
    int clkDiv_;

    esp_err_t configurePort(int port, int memBlockNum, int clkDiv);
    esp_err_t IRTransmitter::uninstallRmtDriver(rmt_channel_t channel);
};
