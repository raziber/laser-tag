#include "encoderHelperFunctions.h"

rmt_item32_t buildItem(uint32_t duration1, uint32_t duration0) {
    rmt_item32_t item;

    item.level0 = 1;
    item.duration0 = duration0;
    item.level1 = 0;
    item.duration1 = duration1;

    printItem(item);

    return item;
}

void printItem(rmt_item32_t item){
    Serial.printf("level0: %d, duration0: %d, level1: %d, duration1: %d\n", item.level0, item.duration0, item.level1, item.duration1);
}

int addBitToItems(rmt_item32_t items[], uint32_t bit, uint32_t index) {
    if (bit) {
        items[index] = buildItem(PAYLOAD_ONE_DURATION_1, PAYLOAD_ONE_DURATION_0);
    } else {
        items[index] = buildItem(PAYLOAD_ZERO_DURATION_1, PAYLOAD_ZERO_DURATION_0);
    }
    return index + 1;
}

int buildDataBits(rmt_item32_t items[], uint32_t data, uint32_t length, bool includeInverted) {
    uint32_t index = 0;

    // Add normal bits
    for (uint32_t i = 0; i < length; i++) {
        bool bit = data & (1 << i);
        index = addBitToItems(items, bit, index);
        //Serial.printf("Data Bit: %d, High: %d, Low: %d\n", bit, items[index - 1].duration0, items[index - 1].duration1);
    }

    // Add inverted bits if necessary
    if (includeInverted) {
        for (uint32_t i = 0; i < length; i++) {
            index = addBitToItems(items, !(data & (1 << i)), index);
        }
    }

    return index;
}

bool buildPacket(rmt_item32_t items[], uint32_t address, uint32_t command){
    uint32_t i = 0;

    // Build header
    items[i++] = buildItem(LEADING_CODE_DURATION_1, LEADING_CODE_DURATION_0);

    // Build data bits
    i += buildDataBits(&items[i], address, ADDRESS_BITS, HAS_INVERTED_ADDRESS_BOOL);

    i += buildDataBits(&items[i], command, COMMAND_BITS, HAS_INVERTED_COMMAND_BOOL);

    #ifdef HAS_STOP_BIT
        // Build stop bit
        items[i++] = buildItem(STOP_BIT_DURATION_1, STOP_BIT_DURATION_0);   // change from 700 to protocol specific value
    #endif

    // check success and return true if successful
    return (i == FRAME_ITEM_COUNT);
}
