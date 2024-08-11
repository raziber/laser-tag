#pragma once

#include "IProtocolSettings.h"

class CustomProtocolSettings : public IProtocolSettings {
public:
    int getDecodeMargin() const override { return 100; }
    int getLeadingCodeDuration0() const override { return 2400; }
    int getLeadingCodeDuration1() const override { return 0; }
    int getPayloadZeroDuration0() const override { return 600; }
    int getPayloadZeroDuration1() const override { return 0; }
    int getPayloadOneDuration0() const override { return 1200; }
    int getPayloadOneDuration1() const override { return 0; }
    int getRepeatCodeDuration0() const override { return 0; }
    int getRepeatCodeDuration1() const override { return 0; }
    bool getHasStopBit() const override { return false; }
    int getStopBitDuration0() const override { return 0; }
    int getStopBitDuration1() const override { return 0; }
    int getLeadingItemCount() const override { return 0; }
    int getRepeatFrameItemCount() const override { return 0; }
    int getAddressBits() const override { return 0; }
    int getCommandBits() const override { return 0; }
    bool getHasInvertedAddress() const override { return false; }
    bool getHasInvertedCommand() const override { return false; }
};
