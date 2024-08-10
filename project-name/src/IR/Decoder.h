#pragma once

#include "driver/rmt.h"
#include "IProtocolSettings.h"

// functions that are general for all decoders

class Decoder{
public:
    Decoder(const IProtocolSettings* settings);
    ~Decoder();

    bool checkInRange(uint32_t duration, uint32_t spec) const;
    bool parseLogic0(rmt_item32_t* item) const;
    bool parseLogic1(rmt_item32_t* item) const;
    bool parseLeadingCode(rmt_item32_t* item) const;
    bool getBit(rmt_item32_t* item, uint16_t& data, int index) const;
    bool checkInvertedBit(rmt_item32_t* item, uint16_t& data, int index) const;
    bool parseFrame(rmt_item32_t* items, uint16_t& address, uint16_t& command) const;
    bool parseRepeatFrame(rmt_item32_t* items) const;
private:
    const IProtocolSettings* settings;
};
