#pragma once

#include <driver/rmt.h>
#include <Arduino.h>
#include <memory>
#include <vector>
#include "IRProtocols/IRProtocolFactory.hpp"

class IRDecoder {
public:
    IRDecoder(std::unique_ptr<IRProtocolSettings> protocolSettings);
    ~IRDecoder();

    esp_err_t decode(const std::vector<rmt_item32_t>& rawData, uint32_t& address, uint32_t& command);

private:
    std::unique_ptr<IRProtocolSettings> protocolSettings_;
    
    esp_err_t decodeProtocol(const std::vector<rmt_item32_t>& items, uint32_t& address, uint32_t& command) const;

    // Helper functions for decoding
    bool matchDuration(uint32_t measured_ticks, uint32_t expected_us) const;
};
