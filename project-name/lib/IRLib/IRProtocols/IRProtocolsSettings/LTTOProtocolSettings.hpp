#pragma once

#include "IProtocolSettings.h"

class LTTOProtocolSettings : public IProtocolSettings {
public:
    u_int32_t getDecodeMargin() const override { return 100; }
    u_int32_t getLeadingCodeDuration0() const override { return 2400; }
    u_int32_t getLeadingCodeDuration1() const override { return 0; }
    u_int32_t getPayloadZeroDuration0() const override { return 600; }
    u_int32_t getPayloadZeroDuration1() const override { return 0; }
    u_int32_t getPayloadOneDuration0() const override { return 1200; }
    u_int32_t getPayloadOneDuration1() const override { return 0; }
    u_int32_t getRepeatCodeDuration0() const override { return 0; }
    u_int32_t getRepeatCodeDuration1() const override { return 0; }
    bool getHasStopBit() const override { return false; }
    u_int32_t getStopBitDuration0() const override { return 0; }
    u_int32_t getStopBitDuration1() const override { return 0; }
    u_int32_t getLeadingItemCount() const override { return 0; }
    u_int32_t getRepeatFrameItemCount() const override { return 0; }
    u_int32_t getAddressBits() const override { return 0; }
    u_int32_t getCommandBits() const override { return 0; }
    bool getHasInvertedAddress() const override { return false; }
    bool getHasInvertedCommand() const override { return false; }
};
