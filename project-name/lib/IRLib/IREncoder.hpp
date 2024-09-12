#pragma once

#include "IRProtocols/IRProtocolFactory.hpp"
#include <driver/rmt.h>
#include <Arduino.h>
#include <vector>
#include <memory>

class IREncoder {
public:
    IREncoder(IRProtocol protocol);
    ~IREncoder();

    esp_err_t createPacket(std::unique_ptr<std::vector<rmt_item32_t>>& packet, uint32_t address, uint32_t command);
private:
    std::unique_ptr<IRProtocolSettings> protocolSettings_;

    uint32_t getBitLength(uint32_t value) const;
    rmt_item32_t createPulseItem(uint32_t duration1, uint32_t duration0) const;
    esp_err_t appendPulseToPacket(std::vector<rmt_item32_t>& packet, bool isOne) const;
    esp_err_t appendHeaderToPacket(std::vector<rmt_item32_t>& packet) const;
    esp_err_t appendDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data) const;
    esp_err_t appendStopToPacket(std::vector<rmt_item32_t>& packet) const;
    esp_err_t appendInvertedDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data) const;

    esp_err_t validatePacketInput(std::unique_ptr<std::vector<rmt_item32_t>>& packet, uint32_t address, uint32_t command) const;
    esp_err_t initializePacket(std::unique_ptr<std::vector<rmt_item32_t>>& packet) const;
    esp_err_t assemblePacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command) const;
    esp_err_t addInvertedDataIfNeeded(std::vector<rmt_item32_t>& packet, uint32_t data, bool needsInversion) const;
};
