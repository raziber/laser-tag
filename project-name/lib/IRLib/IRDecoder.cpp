#include "IRDecoder.hpp"

IRDecoder::IRDecoder(std::unique_ptr<IRProtocolSettings> protocolSettings)
    : protocolSettings_(std::move(protocolSettings)) {
    if (!protocolSettings_) {
        ESP_LOGE("IRDecoder", "Protocol settings not initialized.");
    }
}

IRDecoder::~IRDecoder() {
    // protocolSettings_ is a unique_ptr and will be automatically deleted
}

esp_err_t IRDecoder::decode(const std::vector<rmt_item32_t>& rawData, uint32_t& address, uint32_t& command) {
    if (!protocolSettings_) {
        ESP_LOGE("IRDecoder", "Protocol settings not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    if (rawData.empty()) {
        ESP_LOGE("IRDecoder", "Raw data is empty.");
        return ESP_ERR_INVALID_ARG;
    }

    return decodeProtocol(rawData, address, command);
}

bool IRDecoder::matchDuration(uint32_t measured_ticks, uint32_t expected_us) const {
    // Calculate the duration in microseconds based on the clock divider and RMT clock
    uint32_t measured_us = (measured_ticks * 10) / (protocolSettings_->getClockDivider() / 10);
    uint32_t margin = protocolSettings_->getDecodeMargin();

    return (measured_us >= expected_us - margin) && (measured_us <= expected_us + margin);
}

esp_err_t IRDecoder::decodeProtocol(const std::vector<rmt_item32_t>& items, uint32_t& address, uint32_t& command) const {
    size_t index = 0;
    size_t n_items = items.size();

    // Check for header
    if (n_items < protocolSettings_->getLeadingItemCount()) {
        ESP_LOGE("IRDecoder", "Not enough items for header.");
        return ESP_ERR_INVALID_SIZE;
    }

    // Match leading code
    if (!matchDuration(items[index].duration0, protocolSettings_->getLeadingCodeDuration0()) ||
        !matchDuration(items[index].duration1, protocolSettings_->getLeadingCodeDuration1())) {
        ESP_LOGE("IRDecoder", "Header does not match.");
        return ESP_ERR_INVALID_RESPONSE;
    }
    index++;

    uint32_t data = 0;
    uint32_t bitCount = protocolSettings_->getAddressBits() + protocolSettings_->getCommandBits();
    bool isLsbFirst = protocolSettings_->isLsbFirst();

    for (uint32_t i = 0; i < bitCount; ++i) {
        if (index >= n_items) {
            ESP_LOGE("IRDecoder", "Not enough items for data.");
            return ESP_ERR_INVALID_SIZE;
        }

        // Match payload zero durations
        if (matchDuration(items[index].duration0, protocolSettings_->getPayloadZeroDuration0()) &&
            matchDuration(items[index].duration1, protocolSettings_->getPayloadZeroDuration1())) {
            if (isLsbFirst) {
                data |= (0 << i);
            } else {
                data = (data << 1);
            }
        }
        // Match payload one durations
        else if (matchDuration(items[index].duration0, protocolSettings_->getPayloadOneDuration0()) &&
                 matchDuration(items[index].duration1, protocolSettings_->getPayloadOneDuration1())) {
            if (isLsbFirst) {
                data |= (1 << i);
            } else {
                data = (data << 1) | 1;
            }
        }
        else {
            ESP_LOGE("IRDecoder", "Data pulse does not match at index %d.", index);
            return ESP_ERR_INVALID_RESPONSE;
        }
        index++;
    }

    // Extract address and command from data
    uint32_t addressBits = protocolSettings_->getAddressBits();
    uint32_t commandBits = protocolSettings_->getCommandBits();

    uint32_t addr = data & ((1 << addressBits) - 1);
    uint32_t cmd = (data >> addressBits) & ((1 << commandBits) - 1);

    // Handle inverted address and command if necessary
    if (protocolSettings_->getHasInvertedAddress()) {
        uint32_t inv_addr = (data >> (addressBits * 2)) & ((1 << addressBits) - 1);
        if ((addr ^ inv_addr) != ((1 << addressBits) - 1)) {
            ESP_LOGE("IRDecoder", "Address inversion mismatch.");
            return ESP_ERR_INVALID_RESPONSE;
        }
    }

    if (protocolSettings_->getHasInvertedCommand()) {
        uint32_t inv_cmd = (data >> ((protocolSettings_->getHasInvertedAddress() ? 3 : 2) * addressBits)) & ((1 << commandBits) - 1);
        if ((cmd ^ inv_cmd) != ((1 << commandBits) - 1)) {
            ESP_LOGE("IRDecoder", "Command inversion mismatch.");
            return ESP_ERR_INVALID_RESPONSE;
        }
    }

    // Assign the decoded values
    address = addr;
    command = cmd;

    return ESP_OK;
}
