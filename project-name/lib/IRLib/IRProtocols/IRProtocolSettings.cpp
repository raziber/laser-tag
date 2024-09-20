#include "IRProtocolSettings.hpp"

uint32_t IRProtocolSettings::getAddressItemCount() const {
    return getAddressBits() * (1 + getHasInvertedAddress());
}

uint32_t IRProtocolSettings::getCommandItemCount() const {
    return getCommandBits() * (1 + getHasInvertedCommand());
}

uint32_t IRProtocolSettings::getStopItemCount() const {
    return getHasStopBit();
}

uint32_t IRProtocolSettings::getMaxPacketSize() const {
    return getLeadingItemCount() + getAddressItemCount() + getCommandItemCount() + getStopItemCount();
}
