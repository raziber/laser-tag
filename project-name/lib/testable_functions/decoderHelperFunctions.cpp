#include "decoderHelperFunctions.h"

#include "configurationBackend.h"

bool checkInRange(uint32_t duration, uint32_t spec) {
    return (duration < (spec + irProtocolSettings.decode_margin)) && (duration > (spec - irProtocolSettings.decode_margin));
}

bool parseLogic0(rmt_item32_t* item) {
    return checkInRange(item->duration0, irProtocolSettings.payload_zero_duration_0) &&
           checkInRange(item->duration1, irProtocolSettings.payload_zero_duration_1);
}

bool parseLogic1(rmt_item32_t* item) {
    return checkInRange(item->duration0, irProtocolSettings.payload_one_duration_0) &&
           checkInRange(item->duration1, irProtocolSettings.payload_one_duration_1);
}

bool parseLeadingCode(rmt_item32_t* item) {
    return checkInRange(item->duration0, irProtocolSettings.leading_code_duration_0) &&
           checkInRange(item->duration1, irProtocolSettings.leading_code_duration_1);
}

bool getBit(rmt_item32_t* item, uint16_t& data, int index){
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

bool checkInvertedBit(rmt_item32_t* item, uint16_t& data, int index){
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

bool parseFrame(rmt_item32_t* items, uint16_t& address, uint16_t& command) {
    if (!parseLeadingCode(items++)) return false;

    for (int i = 0; i < irProtocolSettings.address_bits; i++) 
        if(!getBit(items++, address, i)) return false;

    if(irProtocolSettings.has_inverted_address){
        for (int i = 0; i < irProtocolSettings.address_bits; i++)
            if(!checkInvertedBit(items++, address, i)) return false;
    }

    for (int i = 0; i < irProtocolSettings.command_bits; i++)
        if(!getBit(items++, command, i)) return false;

    if(irProtocolSettings.has_inverted_command){
        for (int i = 0; i < irProtocolSettings.command_bits; i++)
            if(!checkInvertedBit(items++, command, i)) return false;
    }

    return true;
}

bool parseRepeatFrame(rmt_item32_t* items) {
    return checkInRange(items->duration0, irProtocolSettings.repeat_code_duration_0) &&
           checkInRange(items->duration1, irProtocolSettings.repeat_code_duration_1);
}
