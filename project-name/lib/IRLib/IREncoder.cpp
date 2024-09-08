#include "IREncoder.hpp"

IREncoder::IREncoder(IRProtocol protocol){
    // Use the factory to load the correct protocol settings
    protocolSettings_ = IRProtocolFactory::createProtocolSettings(protocol);

    if (!protocolSettings_) {
        throw std::invalid_argument("Failed to load protocol settings");
    }
}

IREncoder::~IREncoder(){}

uint32_t IREncoder::getBitLength(uint32_t value) const {
    return value ? static_cast<uint32_t>(log2(value) + 1) : 1;
}

rmt_item32_t IREncoder::createPulseItem(uint32_t duration0, uint32_t duration1) const {
    rmt_item32_t item;

    // the first phase of the pulse is set to HIGH and the second phase is set to LOW
    item.level0 = 1;
    item.duration0 = duration0;
    item.level1 = 0;
    item.duration1 = duration1;

    return item;
}

void IREncoder::appendPulseToPacket(std::vector<rmt_item32_t>& packet, bool isOne) const {
    if (isOne) {
        // Append pulse for '1' to the packet
        packet.push_back(createPulseItem(
            protocolSettings_->getPayloadOneDuration0(),
            protocolSettings_->getPayloadOneDuration1()
        ));
    } else {
        // Append pulse for '0' to the packet
        packet.push_back(createPulseItem(
            protocolSettings_->getPayloadZeroDuration0(),
            protocolSettings_->getPayloadZeroDuration1()
        ));    
    }
}

void IREncoder::appendHeaderToPacket(std::vector<rmt_item32_t>& packet) const {
    packet.push_back(createPulseItem(
            protocolSettings_->getLeadingCodeDuration0(),
            protocolSettings_->getLeadingCodeDuration1()
        ));
}

void IREncoder::appendStopToPacket(std::vector<rmt_item32_t>& packet) const {
    if(protocolSettings_->getHasStopBit()){
        packet.push_back(createPulseItem(
                protocolSettings_->getStopBitDuration0(),
                protocolSettings_->getStopBitDuration1()
            ));
    }
}

void IREncoder::appendDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data) const {
    uint32_t length = getBitLength(data);
    
    for (uint32_t i = 0; i < length; i++) {
        bool bit = data & (1 << i);
        appendPulseToPacket(packet, bit);
    }
}

void IREncoder::appendInvertedDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data) const {
    appendDataToPacket(packet, ~data);
}

std::vector<rmt_item32_t> IREncoder::sendCommand(uint32_t address, uint32_t command){
    // create packet to send
    std::vector<rmt_item32_t> packet;

    appendHeaderToPacket(packet);
    appendDataToPacket(packet, address);
    if(protocolSettings_->getHasInvertedAddress()){appendInvertedDataToPacket(packet, address);}
    appendDataToPacket(packet, command);
    if(protocolSettings_->getHasInvertedCommand()){appendInvertedDataToPacket(packet, command);}
    appendStopToPacket(packet);

    return packet;
}
