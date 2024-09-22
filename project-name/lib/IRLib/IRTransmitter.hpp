#pragma once

#include "IRProtocols/IRProtocolSettings.hpp"
#include "IREncoder.hpp"
#include <driver/rmt.h>
#include <memory>
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


class IRTransmitter{
public:
    IRTransmitter(const std::vector<int>& ports, std::unique_ptr<IREncoder> encoder, int memBlockNum, int clkDiv);
    ~IRTransmitter();

    esp_err_t transmitToAllPorts(uint32_t address, uint32_t command) const;
    esp_err_t transmitToSinglePort(uint32_t portIndex, uint32_t address, uint32_t command) const;
private:
    mutable SemaphoreHandle_t transmitMutex_;
    std::unique_ptr<IREncoder> encoder_;
    std::vector<int> gpioPorts_;
    int memBlockNum_;
    int clkDiv_;

    esp_err_t createPacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command) const;
    esp_err_t transmitToPort(uint32_t portIndex, std::vector<rmt_item32_t>& packet) const;
    esp_err_t validatePortIndex(uint32_t portIndex) const;
    void setPortRmtParams(rmt_config_t& rmt_tx_config, int portIndex, int memBlockNum, int clkDiv) const;

    esp_err_t configurePort(int port, int memBlockNum, int clkDiv);
    esp_err_t uninstallRmtDriver(rmt_channel_t channel);

    // Prevent copying
    IRTransmitter(const IRTransmitter&) = delete;
    IRTransmitter& operator=(const IRTransmitter&) = delete;
};
