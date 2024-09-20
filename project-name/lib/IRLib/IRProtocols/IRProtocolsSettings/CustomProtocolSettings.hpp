#pragma once

#include "IRProtocolSettings.hpp"

class CustomProtocolSettings : public IRProtocolSettings {
public:
    uint32_t getLeadingCodeDuration0() const override { return 2400; }
    uint32_t getDecodeMargin() const override { return 100; }
    uint32_t getLeadingCodeDuration1() const override { return 0; }
    uint32_t getPayloadZeroDuration0() const override { return 600; }
    uint32_t getPayloadZeroDuration1() const override { return 0; }
    uint32_t getPayloadOneDuration0() const override { return 1200; }
    uint32_t getPayloadOneDuration1() const override { return 0; }
    uint32_t getRepeatCodeDuration0() const override { return 0; }
    uint32_t getRepeatCodeDuration1() const override { return 0; }
    bool getHasStopBit() const override { return false; }
    uint32_t getStopBitDuration0() const override { return 0; }
    uint32_t getStopBitDuration1() const override { return 0; }
    uint32_t getLeadingItemCount() const override { return 0; }
    uint32_t getRepeatFrameItemCount() const override { return 0; }
    uint32_t getAddressBits() const override { return 0; }
    uint32_t getCommandBits() const override { return 0; }
    bool getHasInvertedAddress() const override { return false; }
    bool getHasInvertedCommand() const override { return false; }
    bool isLsbFirst() const override { return true; }
};
