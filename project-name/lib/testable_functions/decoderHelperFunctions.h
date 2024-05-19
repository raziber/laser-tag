#ifndef DECODER_HELPER_FUNCTIONS_H
#define DECODER_HELPER_FUNCTIONS_H

// functions that are general for all decoders

bool checkInRange(uint32_t duration, uint32_t spec);
bool parseLogic0(rmt_item32_t* item);
bool parseLogic1(rmt_item32_t* item);
bool parseLeadingCode(rmt_item32_t* item);
bool getBit(rmt_item32_t* item, uint16_t& data, int index);
bool checkInvertedBit(rmt_item32_t* item, uint16_t& data, int index);
bool parseFrame(rmt_item32_t* items, uint16_t& address, uint16_t& command);
bool parseRepeatFrame(rmt_item32_t* items);

#endif // DECODER_HELPER_FUNCTIONS_H