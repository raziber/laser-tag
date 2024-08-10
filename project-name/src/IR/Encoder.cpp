#include "Encoder.h"

#include <Arduino.h>
#include "utils.h"

Encoder::Encoder(const IProtocolSettings* settings) : settings(settings) {}

Encoder::~Encoder(){}

rmt_item32_t Encoder::buildItem(uint32_t duration1, uint32_t duration0) const {
    rmt_item32_t item;

    item.level0 = 1;
    item.duration0 = duration0;
    item.level1 = 0;
    item.duration1 = duration1;

    printItem(item);

    return item;
}

void Encoder::printItem(rmt_item32_t item) const {
    Serial.printf("level0: %d, duration0: %d, level1: %d, duration1: %d\n", item.level0, item.duration0, item.level1, item.duration1);
}

int Encoder::addBitToItems(rmt_item32_t items[], uint32_t bit, uint32_t index) const {
    if (bit) {
        items[index] = buildItem(settings->getPayloadOneDuration1(), settings->getPayloadOneDuration0());
    } else {
        items[index] = buildItem(settings->getPayloadZeroDuration1(), settings->getPayloadZeroDuration0());
    }
    return index + 1;
}

int Encoder::buildDataBits(rmt_item32_t items[], uint32_t data, uint32_t length, bool includeInverted) const {
    uint32_t index = 0;

    // Add normal bits
    for (uint32_t i = 0; i < length; i++) {
        bool bit = data & (1 << i);
        index = addBitToItems(items, bit, index);
    }

    // Add inverted bits if necessary
    if (includeInverted) {
        for (uint32_t i = 0; i < length; i++) {
            index = addBitToItems(items, !(data & (1 << i)), index);
        }
    }

    return index;
}

bool Encoder::buildPacket(rmt_item32_t items[], uint32_t address, uint32_t command) const {
    uint32_t i = 0;

    // Build header
    items[i++] = buildItem(settings->getLeadingCodeDuration1(), settings->getLeadingCodeDuration0());

    // Build data bits
    i += buildDataBits(&items[i], address, settings->getAddressBits(), settings->getHasInvertedAddress());

    i += buildDataBits(&items[i], command, settings->getCommandBits(), settings->getHasInvertedCommand());

    if(settings->getHasStopBit()){
        // Build stop bit
        items[i++] = buildItem(settings->getStopBitDuration1(), settings->getStopBitDuration0());
    }

    // check success and return true if successful
    return (i == settings->getFrameItemCount());
}
