#pragma once

#include "IRProtocols/IRProtocolFactory.hpp"
#include <driver/rmt.h>
#include <Arduino.h>
#include <vector>
#include <memory>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class IREncoder {
public:
    IREncoder(IRProtocol protocol);
    ~IREncoder();

    esp_err_t createPacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command);
private:
    mutable SemaphoreHandle_t encoderMutex_;
    std::unique_ptr<IRProtocolSettings> protocolSettings_;

    rmt_item32_t createPulseItem(uint32_t duration0, uint32_t duration1) const;
    esp_err_t appendPulseToPacket(std::vector<rmt_item32_t>& packet, bool isOne) const;
    esp_err_t appendHeaderToPacket(std::vector<rmt_item32_t>& packet) const;
    esp_err_t appendDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data, uint32_t bitLength) const;
    esp_err_t appendStopToPacket(std::vector<rmt_item32_t>& packet) const;
    esp_err_t appendInvertedDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data, uint32_t bitLength) const;

    esp_err_t validatePacketInput(uint32_t address, uint32_t command) const;
    esp_err_t initializePacket(std::vector<rmt_item32_t>& packet) const;
    esp_err_t assemblePacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command) const;
};
