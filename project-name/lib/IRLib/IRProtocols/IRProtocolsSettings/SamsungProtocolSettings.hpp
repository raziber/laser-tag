#pragma once

#include "IRProtocolSettings.hpp"

class SamsungProtocolSettings : public IRProtocolSettings {
public:
    u_int32_t getDecodeMargin() const override { return 300; }
    u_int32_t getLeadingCodeDuration0() const override { return 4600; }
    u_int32_t getLeadingCodeDuration1() const override { return 4400; }
    u_int32_t getPayloadZeroDuration0() const override { return 700; }
    u_int32_t getPayloadZeroDuration1() const override { return 1570; }
    u_int32_t getPayloadOneDuration0() const override { return 700; }
    u_int32_t getPayloadOneDuration1() const override { return 430; }
    u_int32_t getRepeatCodeDuration0() const override { return 9000; }
    u_int32_t getRepeatCodeDuration1() const override { return 2250; }
    bool getHasStopBit() const override { return true; }
    u_int32_t getStopBitDuration0() const override { return 700; }
    u_int32_t getStopBitDuration1() const override { return 4500; }
    u_int32_t getLeadingItemCount() const override { return 1; }
    u_int32_t getRepeatFrameItemCount() const override { return 4; }
    u_int32_t getAddressBits() const override { return 16; }
    u_int32_t getCommandBits() const override { return 16; }
    bool getHasInvertedAddress() const override { return false; }
    bool getHasInvertedCommand() const override { return false; }
};
