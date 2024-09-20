#pragma once

class IRProtocolSettings{
public:
    virtual ~IRProtocolSettings() = default;

    virtual uint32_t getDecodeMargin() const = 0;
    virtual uint32_t getLeadingCodeDuration0() const = 0;
    virtual uint32_t getLeadingCodeDuration1() const = 0;
    virtual uint32_t getPayloadZeroDuration0() const = 0;
    virtual uint32_t getPayloadZeroDuration1() const = 0;
    virtual uint32_t getPayloadOneDuration0() const = 0;
    virtual uint32_t getPayloadOneDuration1() const = 0;
    virtual uint32_t getRepeatCodeDuration0() const = 0;
    virtual uint32_t getRepeatCodeDuration1() const = 0;
    virtual bool getHasStopBit() const = 0;
    virtual uint32_t getStopBitDuration0() const = 0;
    virtual uint32_t getStopBitDuration1() const = 0;
    virtual uint32_t getLeadingItemCount() const = 0;
    virtual uint32_t getRepeatFrameItemCount() const = 0;
    virtual uint32_t getAddressBits() const = 0;
    virtual uint32_t getCommandBits() const = 0;
    virtual bool getHasInvertedAddress() const = 0;
    virtual bool getHasInvertedCommand() const = 0;
    virtual bool isLsbFirst() const = 0;

    uint32_t getAddressItemCount() const;
    uint32_t getCommandItemCount() const;
    uint32_t getStopItemCount() const;
    uint32_t getMaxPacketSize() const;
};