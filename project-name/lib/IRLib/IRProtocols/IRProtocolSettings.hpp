#pragma once

class IRProtocolSettings{
public:
    virtual ~IRProtocolSettings() = default;

    virtual u_int32_t getDecodeMargin() const = 0;
    virtual u_int32_t getLeadingCodeDuration0() const = 0;
    virtual u_int32_t getLeadingCodeDuration1() const = 0;
    virtual u_int32_t getPayloadZeroDuration0() const = 0;
    virtual u_int32_t getPayloadZeroDuration1() const = 0;
    virtual u_int32_t getPayloadOneDuration0() const = 0;
    virtual u_int32_t getPayloadOneDuration1() const = 0;
    virtual u_int32_t getRepeatCodeDuration0() const = 0;
    virtual u_int32_t getRepeatCodeDuration1() const = 0;
    virtual bool getHasStopBit() const = 0;
    virtual u_int32_t getStopBitDuration0() const = 0;
    virtual u_int32_t getStopBitDuration1() const = 0;
    virtual u_int32_t getLeadingItemCount() const = 0;
    virtual u_int32_t getRepeatFrameItemCount() const = 0;
    virtual u_int32_t getAddressBits() const = 0;
    virtual u_int32_t getCommandBits() const = 0;
    virtual bool getHasInvertedAddress() const = 0;
    virtual bool getHasInvertedCommand() const = 0;
    virtual bool isLsbFirst() const = 0;

    u_int32_t getAddressItemCount() const;
    u_int32_t getCommandItemCount() const;
    u_int32_t getStopItemCount() const;
    u_int32_t getFrameItemCount() const;
};