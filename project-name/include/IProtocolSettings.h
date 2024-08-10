#pragma once

class IProtocolSettings{
public:
    virtual ~IProtocolSettings() = default;

    virtual int getDecodeMargin() const = 0;
    virtual int getLeadingCodeDuration0() const = 0;
    virtual int getLeadingCodeDuration1() const = 0;
    virtual int getPayloadZeroDuration0() const = 0;
    virtual int getPayloadZeroDuration1() const = 0;
    virtual int getPayloadOneDuration0() const = 0;
    virtual int getPayloadOneDuration1() const = 0;
    virtual int getRepeatCodeDuration0() const = 0;
    virtual int getRepeatCodeDuration1() const = 0;
    virtual bool getHasStopBit() const = 0;
    virtual int getStopBit_Duration0() const = 0;
    virtual int getStopBitDuration1() const = 0;
    virtual int getLeadingItemCount() const = 0;
    virtual int getRepeatFrameItemCount() const = 0;
    virtual int getAddressBits() const = 0;
    virtual int getCommandBits() const = 0;
    virtual bool getHasInvertedAddress() const = 0;
    virtual bool getHasInvertedCommand() const = 0;
    virtual int getAddressItemCount() const = 0;
    virtual int getCommandItemCount() const = 0;
    virtual int getStopItemCount() const = 0;
    virtual int getFrameItemCount() const = 0;
};