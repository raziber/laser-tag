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

    std::vector<rmt_item32_t> createPacket(uint32_t address, uint32_t command);
private:
    std::unique_ptr<IRProtocolSettings> protocolSettings_;

    uint32_t getBitLength(uint32_t value) const;
    rmt_item32_t createPulseItem(uint32_t duration1, uint32_t duration0) const;
    void appendPulseToPacket(std::vector<rmt_item32_t>& packet, bool isOne) const;
    void appendHeaderToPacket(std::vector<rmt_item32_t>& packet) const;
    void appendDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data) const;
    void appendStopToPacket(std::vector<rmt_item32_t>& packet) const;
    void appendInvertedDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data) const;
    
};
