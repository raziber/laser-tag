#include "encoderHelperFunctions.h"

#include <Arduino.h>
#include "configurationBackend.h"
#include "utils.h"

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
        items[index] = buildItem(irSettings::irProtocolSettings.payload_one_duration_1, irSettings::irProtocolSettings.payload_one_duration_0);
    } else {
        items[index] = buildItem(irSettings::irProtocolSettings.payload_zero_duration_1, irSettings::irProtocolSettings.payload_zero_duration_0);
    }
    return index + 1;
}

int buildDataBits(rmt_item32_t items[], uint32_t data, uint32_t length, bool includeInverted) {
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

bool buildPacket(rmt_item32_t items[], uint32_t address, uint32_t command){
    uint32_t i = 0;

    // Build header
    items[i++] = buildItem(irSettings::irProtocolSettings.leading_code_duration_1, irSettings::irProtocolSettings.leading_code_duration_0);

    // Build data bits
    i += buildDataBits(&items[i], address, irSettings::irProtocolSettings.address_bits, irSettings::irProtocolSettings.has_inverted_address);

    i += buildDataBits(&items[i], command, irSettings::irProtocolSettings.command_bits, irSettings::irProtocolSettings.has_inverted_command);

    if(irSettings::irProtocolSettings.has_stop_bit){
        // Build stop bit
        items[i++] = buildItem(irSettings::irProtocolSettings.stop_bit_duration_1, irSettings::irProtocolSettings.stop_bit_duration_0);
    }

    // check success and return true if successful
    return (i == irSettings::irProtocolSettings.frame_item_count);
}
