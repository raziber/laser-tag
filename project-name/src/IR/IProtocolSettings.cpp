#include "IProtocolSettings.h"

int IProtocolSettings::getAddressItemCount() const {
    return getAddressBits() * (1 + getHasInvertedAddress());
}

int IProtocolSettings::getCommandItemCount() const {
    return getCommandBits() * (1 + getHasInvertedCommand());
}

int IProtocolSettings::getStopItemCount() const {
    return getHasStopBit();
}

int IProtocolSettings::getFrameItemCount() const {
    return getLeadingItemCount() + getAddressItemCount() + getCommandItemCount() + getStopItemCount();
}