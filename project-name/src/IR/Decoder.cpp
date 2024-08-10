#include "Decoder.h"

Decoder::Decoder(const IProtocolSettings* settings) : settings(settings) {}
Decoder::~Decoder(){}

bool Decoder::checkInRange(uint32_t duration, uint32_t spec) const {
    return (duration < (spec + settings->getDecodeMargin())) && (duration > (spec - settings->getDecodeMargin()));
}

bool Decoder::parseLogic0(rmt_item32_t* item) const {
    return checkInRange(item->duration0, settings->getPayloadZeroDuration0()) &&
            checkInRange(item->duration1, settings->getPayloadZeroDuration1());
}

bool Decoder::parseLogic1(rmt_item32_t* item) const {
    return checkInRange(item->duration0, settings->getPayloadOneDuration0()) &&
            checkInRange(item->duration1, settings->getPayloadOneDuration1());
}

bool Decoder::parseLeadingCode(rmt_item32_t* item) const {
    return checkInRange(item->duration0, settings->getLeadingCodeDuration0()) &&
            checkInRange(item->duration1, settings->getLeadingCodeDuration1());
}

bool Decoder::getBit(rmt_item32_t* item, uint16_t& data, int index) const {
    if (parseLogic1(item)) {
        data |= 1 << index;
        return true;
    } else if (parseLogic0(item)) {
        data &= ~(1 << index);
        return true;
    } else {
        return false;
    }
}

bool Decoder::checkInvertedBit(rmt_item32_t* item, uint16_t& data, int index) const {
    if (parseLogic1(item)) {
        if (data & (1 << index)) {
            return false; // Inverted bit mismatch
        }
        return true;
    } else if (parseLogic0(item)) {
        if (!(data & (1 << index))) {
            return false; // Inverted bit mismatch
        }
        return true;
    } else {
        return false;
    }
}

bool Decoder::parseFrame(rmt_item32_t* items, uint16_t& address, uint16_t& command) const {
    if (!parseLeadingCode(items++)) return false;

    for (int i = 0; i < settings->getAddressBits(); i++) 
        if(!getBit(items++, address, i)) return false;

    if(settings->getHasInvertedAddress()){
        for (int i = 0; i < settings->getAddressBits(); i++)
            if(!checkInvertedBit(items++, address, i)) return false;
    }

    for (int i = 0; i < settings->getCommandBits(); i++)
        if(!getBit(items++, command, i)) return false;

    if(settings->getHasInvertedCommand()){
        for (int i = 0; i < settings->getCommandBits(); i++)
            if(!checkInvertedBit(items++, command, i)) return false;
    }

    return true;
}

bool Decoder::parseRepeatFrame(rmt_item32_t* items) const {
    return checkInRange(items->duration0, settings->getRepeatCodeDuration0()) &&
            checkInRange(items->duration1, settings->getRepeatCodeDuration1());
}
