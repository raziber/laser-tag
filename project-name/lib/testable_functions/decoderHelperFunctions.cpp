#include "decoderHelperFunctions.h"

bool checkInRange(uint32_t duration, uint32_t spec) {
    return (duration < (spec + DECODE_MARGIN)) && (duration > (spec - DECODE_MARGIN));
}

bool parseLogic0(rmt_item32_t* item) {
    return checkInRange(item->duration0, PAYLOAD_ZERO_DURATION_0) &&
           checkInRange(item->duration1, PAYLOAD_ZERO_DURATION_1);
}

bool parseLogic1(rmt_item32_t* item) {
    return checkInRange(item->duration0, PAYLOAD_ONE_DURATION_0) &&
           checkInRange(item->duration1, PAYLOAD_ONE_DURATION_1);
}

bool parseLeadingCode(rmt_item32_t* item) {
    return checkInRange(item->duration0, LEADING_CODE_DURATION_0) &&
           checkInRange(item->duration1, LEADING_CODE_DURATION_1);
}

bool getBit(rmt_item32_t* item, uint16_t& data, int index){
    //Serial.printf("duration0=%d, duration1=%d\n", item->duration0, item->duration1);
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
    //Serial.printf("duration0=%d, duration1=%d\n", (items-1)->duration0, (items-1)->duration1);

    for (int i = 0; i < ADDRESS_BITS; i++) 
        if(!getBit(items++, address, i)) return false;

    #ifdef HAS_INVERTED_ADDRESS
        for (int i = 0; i < ADDRESS_BITS; i++)
            if(!checkInvertedBit(items++, address, i)) return false;
    #endif // HAS_INVERTED_ADDRESS

    for (int i = 0; i < COMMAND_BITS; i++)
        if(!getBit(items++, command, i)) return false;

    #ifdef HAS_INVERTED_COMMAND
        for (int i = 0; i < COMMAND_BITS; i++)
            if(!checkInvertedBit(items++, command, i)) return false;
    #endif // HAS_INVERTED_COMMAND

    return true;
}

bool parseRepeatFrame(rmt_item32_t* items) {
    return checkInRange(items->duration0, REPEAT_CODE_DURATION_0) &&
           checkInRange(items->duration1, REPEAT_CODE_DURATION_1);
}
