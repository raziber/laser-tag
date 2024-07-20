#pragma omce

#include "driver/rmt.h"

// functions that are general for all encoders

rmt_item32_t buildItem(uint32_t high_us, uint32_t low_us);
int addBitToItems(rmt_item32_t items[], uint32_t bit, uint32_t index);
int buildDataBits(rmt_item32_t items[], uint32_t data, uint32_t length, bool includeInverted = false);
bool buildPacket(rmt_item32_t items[], uint32_t address, uint32_t command);
void printItem(rmt_item32_t item);
