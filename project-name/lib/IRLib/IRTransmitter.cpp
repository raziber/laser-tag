#include "IRTransmitter.hpp"

IRTransmitter::IRTransmitter(const std::vector<int>& ports, std::unique_ptr<IREncoder> encoder, std::unique_ptr<IRProtocolSettings> protocolSettings, int memBlockNum, int clkDiv)
        : gpioPorts_(ports), encoder_(std::move(encoder)), protocolSettings_(std::move(protocolSettings)), memBlockNum_(memBlockNum), clkDiv_(clkDiv) {
    
    for (std::size_t i = 0; i < gpioPorts_.size(); ++i) {
        esp_err_t err = configurePort(i, memBlockNum, clkDiv);
        if(err != ESP_OK){
            ESP_LOGE("IRTransmitter", "Failed to configure port %d: %s", i, esp_err_to_name(err));
            // keep configuring other ports
        }
    }
}

IRTransmitter::~IRTransmitter() {
    for (std::size_t i = 0; i < gpioPorts_.size(); ++i) {
        esp_err_t err = uninstallRmtDriver(static_cast<rmt_channel_t>(i));
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
            ESP_LOGW("IRTransmitter", "Failed to uninstall RMT driver for port %d: %s", i, esp_err_to_name(err));
        }    
    }
}

esp_err_t IRTransmitter::createPacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command) const {
    esp_err_t err = encoder_->createPacket(packet, address, command);
    
    if (err != ESP_OK || packet.empty()) {
        ESP_LOGE("IRTransmitter", "Failed to create packet for transmission: %s", esp_err_to_name(err));
        return ESP_ERR_NO_MEM;
    }

    return err;
}

esp_err_t IRTransmitter::transmitToAllPorts(uint32_t address, uint32_t command) const {
    std::vector<rmt_item32_t> packet;
    esp_err_t err = IRTransmitter::createPacket(packet, address, command);

    static constexpr int MAX_RETRIES = 3;
    static constexpr int TIMEOUT_MS = 200;
    esp_err_t final_result = ESP_OK;

    for (size_t i = 0; i < gpioPorts_.size(); ++i) {
        int retries = MAX_RETRIES;
        
        do {
            // Write items to the RMT channel
            err = rmt_write_items(static_cast<rmt_channel_t>(i), packet.data(), packet.size(), true);
            
            if (err == ESP_OK) {
                // Wait for the transmission to finish
                err = rmt_wait_tx_done(static_cast<rmt_channel_t>(i), pdMS_TO_TICKS(TIMEOUT_MS));
                if (err == ESP_OK) {
                    break;  // Success, exit retry loop
                }
            }

            ESP_LOGW("IRTransmitter", "Transmission failed on channel %d, retrying... (%d retries left)", i, retries);

        } while (retries-- > 0);

        if (err != ESP_OK) {
            ESP_LOGE("IRTransmitter", "Failed to transmit on channel %d after retries: %s", i, esp_err_to_name(err));
            final_result = err;  // Track the last error encountered
        }
    }

    return final_result;  // Return the final result
}

esp_err_t IRTransmitter::validatePortIndex(uint32_t portIndex) const {
    if (portIndex >= gpioPorts_.size()) {
        ESP_LOGE("IRTransmitter", "Invalid port index: %d", portIndex);
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t IRTransmitter::transmitToSinglePort(uint32_t portIndex, uint32_t address, uint32_t command) const {
    esp_err_t err = IRTransmitter::validatePortIndex(portIndex);
    if (err != ESP_OK) return err;

    std::vector<rmt_item32_t> packet;
    err = IRTransmitter::createPacket(packet, address, command);
    if (err != ESP_OK) return err;
    
    err = IRTransmitter::transmitToPort(portIndex, packet);
    return err;
}

esp_err_t IRTransmitter::transmitToPort(uint32_t portIndex, std::vector<rmt_item32_t>& packet) const {
    static constexpr int MAX_RETRIES = 3;
    static constexpr int TIMEOUT_MS = 200;
    esp_err_t err = ESP_OK;

    for (int retries = MAX_RETRIES; retries > 0; --retries) {
        err = rmt_write_items(static_cast<rmt_channel_t>(portIndex), packet.data(), packet.size(), true);
        if (err == ESP_OK) {
            err = rmt_wait_tx_done(static_cast<rmt_channel_t>(portIndex), pdMS_TO_TICKS(TIMEOUT_MS));
            if (err == ESP_OK) {
                return ESP_OK;  // Transmission successful
            }
        }
        ESP_LOGW("IRTransmitter", "Transmission failed on channel %d, retrying... (%d retries left)", portIndex, retries - 1);
    }

    ESP_LOGE("IRTransmitter", "Failed to transmit on channel %d after retries: %s", portIndex, esp_err_to_name(err));
    return err;
}

esp_err_t IRTransmitter::configurePort(int portIndex, int memBlockNum, int clkDiv) {
    // Validate port index
    if (portIndex >= gpioPorts_.size()) {
        ESP_LOGE("IRTransmitter", "Invalid port index: %d", portIndex);
        return ESP_ERR_INVALID_ARG;
    }

    rmt_config_t rmt_tx_config;
    rmt_tx_config.rmt_mode = RMT_MODE_TX;
    rmt_tx_config.channel = static_cast<rmt_channel_t>(portIndex);
    rmt_tx_config.gpio_num = static_cast<gpio_num_t>(gpioPorts_[portIndex]);
    rmt_tx_config.mem_block_num = memBlockNum;
    rmt_tx_config.clk_div = clkDiv;

    // Attempt to uninstall the driver and continue if uninstall is non-critical
    esp_err_t err = uninstallRmtDriver(rmt_tx_config.channel);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGW("IRTransmitter", "Failed to uninstall RMT driver for port %d: %s", portIndex, esp_err_to_name(err));
        // Optionally track this error and continue
    }

    // Configure the RMT channel
    err = rmt_config(&rmt_tx_config);
    if (err != ESP_OK) {
        ESP_LOGE("IRTransmitter", "Failed to configure RMT for port %d: %s", portIndex, esp_err_to_name(err));
        return err;
    }

    // Install the RMT driver
    err = rmt_driver_install(rmt_tx_config.channel, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGE("IRTransmitter", "Failed to install RMT driver for port %d: %s", portIndex, esp_err_to_name(err));
        return err;
    }

    ESP_LOGI("IRTransmitter", "Port %d configured successfully", portIndex);
    return ESP_OK;
}

esp_err_t IRTransmitter::uninstallRmtDriver(rmt_channel_t channel) {
    // Attempt to uninstall the RMT driver
    esp_err_t err = rmt_driver_uninstall(channel);

    // Handle success or specific errors
    if (err == ESP_OK) {
        ESP_LOGI("IRTransmitter", "RMT driver uninstalled successfully for channel %d", channel);
    } else if (err == ESP_ERR_INVALID_STATE) {
        ESP_LOGW("IRTransmitter", "RMT driver for channel %d was not installed", channel);
        return ESP_OK;  // Return OK since the driver wasn't installed, so no issue
    } else {
        ESP_LOGE("IRTransmitter", "Failed to uninstall RMT driver for channel %d: %s", channel, esp_err_to_name(err));
        return err;  // Return the actual error for the caller to handle
    }

    return ESP_OK;  // Return OK if uninstalled successfully
}
