#include "IREncoder.hpp"

IREncoder::IREncoder(IRProtocol protocol){
    // Use the factory to load the correct protocol settings
    protocolSettings_ = IRProtocolFactory::createProtocolSettings(protocol);

    if (!protocolSettings_) {
        ESP_LOGE("IREncoder", "Failed to create protocol settings.");
    }
}

IREncoder::~IREncoder(){}

rmt_item32_t IREncoder::createPulseItem(uint32_t duration0, uint32_t duration1) const {
    rmt_item32_t item;

    // the first phase of the pulse is set to HIGH and the second phase is set to LOW
    item.level0 = HIGH;
    item.duration0 = duration0;
    item.level1 = LOW;
    item.duration1 = duration1;

    return item;
}

esp_err_t IREncoder::appendPulseToPacket(std::vector<rmt_item32_t>& packet, bool isOne) const {
    rmt_item32_t pulseItem;

    // Determine pulse durations based on the bit value (1 or 0)
    if (isOne) {
        pulseItem = createPulseItem(
            protocolSettings_->getPayloadOneDuration0(),
            protocolSettings_->getPayloadOneDuration1()
        );
    } else {
        pulseItem = createPulseItem(
            protocolSettings_->getPayloadZeroDuration0(),
            protocolSettings_->getPayloadZeroDuration1()
        );
    }

    packet.push_back(pulseItem);
    return ESP_OK;
}

esp_err_t IREncoder::appendHeaderToPacket(std::vector<rmt_item32_t>& packet) const {
    rmt_item32_t headerPulse = createPulseItem(
        protocolSettings_->getLeadingCodeDuration0(),
        protocolSettings_->getLeadingCodeDuration1()
    );
    
    packet.push_back(headerPulse);
    return ESP_OK;
}

esp_err_t IREncoder::appendStopToPacket(std::vector<rmt_item32_t>& packet) const {
    if (protocolSettings_->getHasStopBit()) {
        rmt_item32_t stopPulse = createPulseItem(
            protocolSettings_->getStopBitDuration0(),
            protocolSettings_->getStopBitDuration1()
        );

        packet.push_back(stopPulse);
    }

    return ESP_OK;
}

esp_err_t IREncoder::appendDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data, uint32_t bitLength) const {
    bool isLsbFirst = protocolSettings_->isLsbFirst();

    for (uint32_t i = 0; i < bitLength; i++) {
        bool bit;
        if (isLsbFirst) {
            bit = (data >> i) & 0x01;
        } else {
            bit = (data >> (bitLength - 1 - i)) & 0x01;
        }

        esp_err_t err = appendPulseToPacket(packet, bit);
        if (err != ESP_OK) {
            ESP_LOGE("IREncoder", "Failed to append pulse to packet at bit %d", i);
            return err;
        }
    }

    return ESP_OK;
}

esp_err_t IREncoder::appendInvertedDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data, uint32_t bitLength) const {
    uint32_t mask = (1 << bitLength) - 1;
    uint32_t invertedData = (~data) & mask;

    esp_err_t err = appendDataToPacket(packet, invertedData, bitLength);
    if (err != ESP_OK) {
        ESP_LOGE("IREncoder", "Failed to append inverted data to packet");
        return err;
    }
    return ESP_OK;
}

esp_err_t IREncoder::createPacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command) {
    ESP_LOGI("IREncoder", "Creating packet for address: 0x%x, command: 0x%x", address, command);

    // Validate the input
    esp_err_t err = validatePacketInput(address, command);
    if (err != ESP_OK) return err;

    // Initialize the packet
    err = initializePacket(packet);
    if (err != ESP_OK) return err;

    // Build the packet
    err = assemblePacket(packet, address, command);
    if (err != ESP_OK) return err;

    ESP_LOGI("IREncoder", "Packet creation complete with %zu items", packet.size());
    return ESP_OK;
}

esp_err_t IREncoder::validatePacketInput(uint32_t address, uint32_t command) const {
    uint32_t addressBits = protocolSettings_->getAddressBits();
    uint32_t commandBits = protocolSettings_->getCommandBits();

    uint32_t maxAddressValue = (1 << addressBits) - 1;
    if (address > maxAddressValue) {
        ESP_LOGE("IREncoder", "Address 0x%x exceeds the maximum allowed value 0x%x for %u bits", address, maxAddressValue, addressBits);
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t maxCommandValue = (1 << commandBits) - 1;
    if (command > maxCommandValue) {
        ESP_LOGE("IREncoder", "Command 0x%x exceeds the maximum allowed value 0x%x for %u bits", command, maxCommandValue, commandBits);
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t IREncoder::initializePacket(std::vector<rmt_item32_t>& packet) const {
    packet.clear();
    size_t maxPacketSize = protocolSettings_->getMaxPacketSize();
    packet.reserve(maxPacketSize);
    ESP_LOGD("IREncoder", "Cleared packet contents and reserved capacity for %zu items", maxPacketSize);
    return ESP_OK;
}

esp_err_t IREncoder::initializePacket(std::vector<rmt_item32_t>& packet) const {
    packet.clear();
    ESP_LOGI("IREncoder", "Cleared packet contents");

    return ESP_OK;
}

esp_err_t IREncoder::assemblePacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command) const {
    // Append header
    esp_err_t err = appendHeaderToPacket(packet);
    if (err != ESP_OK) {
        ESP_LOGE("IREncoder", "Failed to append header to packet");
        return err;
    }

    // Append address
    err = appendDataToPacket(packet, address, protocolSettings_->getAddressBits());
    if (err != ESP_OK) return err;

    // Append inverted address if needed
    if (protocolSettings_->getHasInvertedAddress()) {
        err = appendInvertedDataToPacket(packet, address, protocolSettings_->getAddressBits());
        if (err != ESP_OK) return err;
    }

    // Append command
    err = appendDataToPacket(packet, command, protocolSettings_->getCommandBits());
    if (err != ESP_OK) return err;

    // Append inverted command if needed
    if (protocolSettings_->getHasInvertedCommand()) {
        err = appendInvertedDataToPacket(packet, command, protocolSettings_->getCommandBits());
        if (err != ESP_OK) return err;
    }

    // Append stop bit
    err = appendStopToPacket(packet);
    if (err != ESP_OK) {
        ESP_LOGE("IREncoder", "Failed to append stop bit to packet");
        return err;
    }

    return ESP_OK;
}
