#pragma once

#include "IRProtocolSettings.hpp"

class SamsungProtocolSettings : public IRProtocolSettings {
public:
    uint32_t getDecodeMargin() const override { return 300; }
    uint32_t getLeadingCodeDuration0() const override { return 4600; }
    uint32_t getLeadingCodeDuration1() const override { return 4400; }
    uint32_t getPayloadZeroDuration0() const override { return 700; }
    uint32_t getPayloadZeroDuration1() const override { return 1570; }
    uint32_t getPayloadOneDuration0() const override { return 700; }
    uint32_t getPayloadOneDuration1() const override { return 430; }
    uint32_t getRepeatCodeDuration0() const override { return 9000; }
    uint32_t getRepeatCodeDuration1() const override { return 2250; }
    bool getHasStopBit() const override { return true; }
    uint32_t getStopBitDuration0() const override { return 700; }
    uint32_t getStopBitDuration1() const override { return 4500; }
    uint32_t getLeadingItemCount() const override { return 1; }
    uint32_t getRepeatFrameItemCount() const override { return 4; }
    uint32_t getAddressBits() const override { return 16; }
    uint32_t getCommandBits() const override { return 16; }
    
    uint32_t getFilterTicksThreshold() const override { return 100; }
    uint32_t getIdleThreshold() const override { return 1200; }
    uint32_t getClockDivider() const override { return 80; }

    bool getHasInvertedAddress() const override { return false; }
    bool getHasInvertedCommand() const override { return false; }
    bool isLsbFirst() const override { return false; }
};
