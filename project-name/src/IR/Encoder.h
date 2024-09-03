#pragma once

#include "driver/rmt.h"
#include "IProtocolSettings.h"

class Encoder {
public:
    Encoder(const IProtocolSettings* settings);
    ~Encoder();

    rmt_item32_t buildItem(uint32_t high_us, uint32_t low_us) const;
    int addBitToItems(rmt_item32_t items[], uint32_t bit, uint32_t index) const;
    int buildDataBits(rmt_item32_t items[], uint32_t data, uint32_t length, bool includeInverted = false) const;
    bool buildPacket(rmt_item32_t items[], uint32_t address, uint32_t command) const;
    void printItem(rmt_item32_t item) const;
    
private:
    const IProtocolSettings* settings;
};
