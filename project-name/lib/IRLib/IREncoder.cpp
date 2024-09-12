#include "IREncoder.hpp"

IREncoder::IREncoder(IRProtocol protocol){
    // Use the factory to load the correct protocol settings
    protocolSettings_ = IRProtocolFactory::createProtocolSettings(protocol);

    // TODO: add error management
}

IREncoder::~IREncoder(){}

uint32_t IREncoder::getBitLength(uint32_t value) const {
    uint32_t bitLength = 0;
    while (value) {
        value >>= 1;  // Shift right by one bit
        bitLength++;
    }
    return bitLength > 0 ? bitLength : 1;  // Ensure we return at least 1 for 0
}

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
    if (!protocolSettings_) {
        ESP_LOGE("IREncoder", "Protocol settings are not initialized.");
        return ESP_FAIL;
    }

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

    // Append the pulse to the packet and check if push_back succeeded (for error handling)
    try {
        packet.push_back(pulseItem);
    } catch (const std::bad_alloc& e) {
        ESP_LOGE("IREncoder", "Failed to allocate memory for packet: %s", e.what());
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

esp_err_t IREncoder::appendHeaderToPacket(std::vector<rmt_item32_t>& packet) const {
    if (!protocolSettings_) {
        ESP_LOGE("IREncoder", "Protocol settings are not initialized.");
        return ESP_FAIL;
    }

    rmt_item32_t headerPulse = createPulseItem(
        protocolSettings_->getLeadingCodeDuration0(),
        protocolSettings_->getLeadingCodeDuration1()
    );

    try {
        packet.push_back(headerPulse);
    } catch (const std::bad_alloc& e) {
        ESP_LOGE("IREncoder", "Failed to allocate memory for packet: %s", e.what());
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

esp_err_t IREncoder::appendStopToPacket(std::vector<rmt_item32_t>& packet) const {
    if (!protocolSettings_) {
        ESP_LOGE("IREncoder", "Protocol settings are not initialized.");
        return ESP_FAIL;
    }

    if (protocolSettings_->getHasStopBit()) {
        rmt_item32_t stopPulse = createPulseItem(
            protocolSettings_->getStopBitDuration0(),
            protocolSettings_->getStopBitDuration1()
        );

        try {
            packet.push_back(stopPulse);
        } catch (const std::bad_alloc& e) {
            ESP_LOGE("IREncoder", "Failed to allocate memory for stop pulse: %s", e.what());
            return ESP_ERR_NO_MEM;
        }
    }

    return ESP_OK;
}

esp_err_t IREncoder::appendDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data) const {
    uint32_t length = getBitLength(data);
    
    for (uint32_t i = 0; i < length; i++) {
        bool bit = data & (1 << i);
        esp_err_t err = appendPulseToPacket(packet, bit);
        if (err != ESP_OK) {
            ESP_LOGE("IREncoder", "Failed to append pulse to packet at bit %d", i);
            return err;  // Propagate the error upwards
        }
    }

    return ESP_OK;  // Return success if all pulses were appended successfully
}

esp_err_t IREncoder::appendInvertedDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data) const {
    esp_err_t err = appendDataToPacket(packet, ~data);
    if (err != ESP_OK) {
        ESP_LOGE("IREncoder", "Failed to append inverted data to packet");
        return err;  // Propagate the error upwards
    }
    return ESP_OK;
}

esp_err_t IREncoder::createPacket(std::unique_ptr<std::vector<rmt_item32_t>>& packet, uint32_t address, uint32_t command) {
    ESP_LOGI("IREncoder", "Creating packet for address: 0x%x, command: 0x%x", address, command);

    // Validate the input
    esp_err_t err = validatePacketInput(packet, address, command);
    if (err != ESP_OK) return err;

    // Initialize the packet
    err = initializePacket(packet);
    if (err != ESP_OK) return err;

    // Build the packet
    err = assemblePacket(*packet, address, command);
    if (err != ESP_OK) return err;

    ESP_LOGI("IREncoder", "Packet creation complete with %zu items", packet->size());
    return ESP_OK;
}

esp_err_t IREncoder::validatePacketInput(std::unique_ptr<std::vector<rmt_item32_t>>& packet, uint32_t address, uint32_t command) const {
    // Check if the address fits within the allowed bit length for the selected protocol
    uint32_t addressBits = protocolSettings_->getAddressBits();
    if (getBitLength(address) > addressBits) {
        ESP_LOGE("IREncoder", "Address 0x%x exceeds the allowed bit length of %u bits", address, addressBits);
        return ESP_ERR_INVALID_ARG;
    }

    // Check if the command fits within the allowed bit length for the selected protocol
    uint32_t commandBits = protocolSettings_->getCommandBits();
    if (getBitLength(command) > commandBits) {
        ESP_LOGE("IREncoder", "Command 0x%x exceeds the allowed bit length of %u bits", command, commandBits);
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t IREncoder::initializePacket(std::unique_ptr<std::vector<rmt_item32_t>>& packet) const {
    // Ensure the packet is allocated
    if (!packet) {
        packet = std::make_unique<std::vector<rmt_item32_t>>();
        if (!packet) {
            ESP_LOGE("IREncoder", "Failed to allocate memory for packet");
            return ESP_ERR_NO_MEM;
        }
        ESP_LOGI("IREncoder", "Allocated new packet vector");
    }

    // Clear the packet
    packet->clear();
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
    err = appendDataToPacket(packet, address);
    if (err != ESP_OK) {
        ESP_LOGE("IREncoder", "Failed to append address 0x%x to packet", address);
        return err;
    }

    // Append inverted address if needed
    err = addInvertedDataIfNeeded(packet, address, protocolSettings_->getHasInvertedAddress());
    if (err != ESP_OK) return err;

    // Append command
    err = appendDataToPacket(packet, command);
    if (err != ESP_OK) {
        ESP_LOGE("IREncoder", "Failed to append command 0x%x to packet", command);
        return err;
    }

    // Append inverted command if needed
    err = addInvertedDataIfNeeded(packet, command, protocolSettings_->getHasInvertedCommand());
    if (err != ESP_OK) return err;

    // Append stop bit
    err = appendStopToPacket(packet);
    if (err != ESP_OK) {
        ESP_LOGE("IREncoder", "Failed to append stop bit to packet");
        return err;
    }

    return ESP_OK;
}

esp_err_t IREncoder::addInvertedDataIfNeeded(std::vector<rmt_item32_t>& packet, uint32_t data, bool needsInversion) const {
    if (needsInversion) {
        esp_err_t err = appendInvertedDataToPacket(packet, data);
        if (err != ESP_OK) {
            ESP_LOGE("IREncoder", "Failed to append inverted data 0x%x to packet", ~data);
            return err;
        }
        ESP_LOGI("IREncoder", "Appended inverted data 0x%x to packet", ~data);
    }
    return ESP_OK;
}
