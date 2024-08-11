#pragma once

#include "IProtocolSettings.h"

class NECProtocolSettings : public IProtocolSettings {
public:
    int getDecodeMargin() const override { return 300; }
    int getLeadingCodeDuration0() const override { return 9000; }
    int getLeadingCodeDuration1() const override { return 4500; }
    int getPayloadZeroDuration0() const override { return 560; }
    int getPayloadZeroDuration1() const override { return 560; }
    int getPayloadOneDuration0() const override { return 560; }
    int getPayloadOneDuration1() const override { return 1690; }
    int getRepeatCodeDuration0() const override { return 9000; }
    int getRepeatCodeDuration1() const override { return 2250; }
    bool getHasStopBit() const override { return true; }
    int getStopBitDuration0() const override { return 700; }
    int getStopBitDuration1() const override { return 4500; }
    int getLeadingItemCount() const override { return 1; }
    int getRepeatFrameItemCount() const override { return 2; }
    int getAddressBits() const override { return 8; }
    int getCommandBits() const override { return 8; }
    bool getHasInvertedAddress() const override { return true; }
    bool getHasInvertedCommand() const override { return true; }
};
