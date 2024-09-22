#include "IRTransmitter.hpp"

IRTransmitter::IRTransmitter(const std::vector<int>& ports, std::unique_ptr<IREncoder> encoder, int memBlockNum, int clkDiv)
        : gpioPorts_(ports), encoder_(std::move(encoder)), memBlockNum_(memBlockNum), clkDiv_(clkDiv) {
    
    transmitMutex_ = xSemaphoreCreateMutex();
    if (transmitMutex_ == nullptr) {
        ESP_LOGE("IRTransmitter", "Failed to create mutex");
    }
    
    for (std::size_t i = 0; i < this->gpioPorts_.size(); ++i) {
        esp_err_t err = configurePort(i, memBlockNum, clkDiv);
        if(err != ESP_OK){
            ESP_LOGE("IRTransmitter", "Failed to configure port %d: %s", i, esp_err_to_name(err));
            // keep configuring other ports
        }
    }
}

IRTransmitter::~IRTransmitter() {
    for (std::size_t i = 0; i < gpioPorts_.size(); ++i) {
        rmt_channel_t channel = static_cast<rmt_channel_t>(i);
        esp_err_t err = uninstallRmtDriver(channel);
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
            ESP_LOGW("IRTransmitter", "Failed to uninstall RMT driver for channel %d: %s", channel, esp_err_to_name(err));
        }
    }

    if (transmitMutex_ != nullptr) {
        vSemaphoreDelete(transmitMutex_);
        transmitMutex_ = nullptr;
    }
}

esp_err_t IRTransmitter::createPacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command) const {
    esp_err_t err = encoder_->createPacket(packet, address, command);

    if (err != ESP_OK) {
        ESP_LOGE("IRTransmitter", "Failed to create packet for transmission: %s", esp_err_to_name(err));
        return err;
    }
    if (packet.empty()) {
        ESP_LOGE("IRTransmitter", "Packet is empty after creation.");
        return ESP_ERR_INVALID_SIZE;
    }

    return ESP_OK;
}

esp_err_t IRTransmitter::transmitToAllPorts(uint32_t address, uint32_t command) const {
    if (transmitMutex_ == nullptr) {
        ESP_LOGE("IRTransmitter", "Transmitter mutex not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(transmitMutex_, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE("IRTransmitter", "Failed to take mutex");
        return ESP_FAIL;
    }
    
    // Create the packet using the encoder
    std::vector<rmt_item32_t> packet;
    esp_err_t err = createPacket(packet, address, command);
    if (err != ESP_OK) {
        xSemaphoreGive(transmitMutex_);
        return err;
    }

    // Transmit the packet to all ports
    for (size_t i = 0; i < gpioPorts_.size(); ++i)
    {
        err = transmitToPort(i, packet);
        if (err != ESP_OK) {
            xSemaphoreGive(transmitMutex_);
            return err;
        }
    }
    
    // Release the mutex after operations are complete
    xSemaphoreGive(transmitMutex_);
    return ESP_OK;
}

esp_err_t IRTransmitter::validatePortIndex(uint32_t portIndex) const {
    if (portIndex >= gpioPorts_.size() || portIndex >= RMT_CHANNEL_MAX) {
        ESP_LOGE("IRTransmitter", "Invalid port index: %d", portIndex);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t IRTransmitter::transmitToSinglePort(uint32_t portIndex, uint32_t address, uint32_t command) const {
    if (transmitMutex_ == nullptr) {
        ESP_LOGE("IRTransmitter", "Transmitter mutex not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(transmitMutex_, portMAX_DELAY) != pdTRUE){
        ESP_LOGE("IRTransmitter", "Failed to take mutex");
        return ESP_FAIL;
    }

    esp_err_t err = IRTransmitter::validatePortIndex(portIndex);
    if (err != ESP_OK) {
        xSemaphoreGive(transmitMutex_);  // Release the mutex before returning
        return err;
    }

    std::vector<rmt_item32_t> packet;
    err = IRTransmitter::createPacket(packet, address, command);
    if (err != ESP_OK) {
        xSemaphoreGive(transmitMutex_);  // Release the mutex before returning
        return err;
    }

    err = IRTransmitter::transmitToPort(portIndex, packet);
    xSemaphoreGive(transmitMutex_);  // Release the mutex before returning
    return ESP_OK;
}

esp_err_t IRTransmitter::transmitToPort(uint32_t portIndex, std::vector<rmt_item32_t>& packet) const {
    esp_err_t err = IRTransmitter::validatePortIndex(portIndex);
    if (err != ESP_OK) return err;
    rmt_channel_t channel = static_cast<rmt_channel_t>(portIndex);

    static constexpr int MAX_RETRIES = 3;
    static constexpr int TIMEOUT_MS = 200;

    for (int retries = MAX_RETRIES; retries > 0; --retries) {
        err = rmt_write_items(channel, packet.data(), packet.size(), true);
        if (err == ESP_OK) {
            err = rmt_wait_tx_done(channel, pdMS_TO_TICKS(TIMEOUT_MS));
            if (err == ESP_OK) {
                return ESP_OK;
            }
        }
        ESP_LOGW("IRTransmitter", "Transmission failed on channel %d, retrying... (%d retries left)", portIndex, retries - 1);
        vTaskDelay(pdMS_TO_TICKS(100));  // Delay before retrying
    }

    ESP_LOGE("IRTransmitter", "Failed to transmit on channel %d after retries: %s", portIndex, esp_err_to_name(err));
    return err;
}

void IRTransmitter::setPortRmtParams(rmt_config_t& rmt_tx_config, int portIndex, int memBlockNum, int clkDiv) const {
    rmt_tx_config.rmt_mode = RMT_MODE_TX;
    rmt_tx_config.channel = static_cast<rmt_channel_t>(portIndex);
    rmt_tx_config.gpio_num = static_cast<gpio_num_t>(gpioPorts_[portIndex]);
    rmt_tx_config.mem_block_num = memBlockNum;
    rmt_tx_config.clk_div = clkDiv;
}

esp_err_t IRTransmitter::configurePort(int portIndex, int memBlockNum, int clkDiv) {
    if (transmitMutex_ == nullptr) {
        ESP_LOGE("IRTransmitter", "Transmitter mutex not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(transmitMutex_, portMAX_DELAY) != pdTRUE){
        ESP_LOGE("IRTransmitter", "Failed to take mutex");
        return ESP_FAIL;
    }

    esp_err_t err = IRTransmitter::validatePortIndex(portIndex);
    if (err != ESP_OK) {
        xSemaphoreGive(transmitMutex_);
        return err;
    }

    rmt_config_t rmt_tx_config;
    IRTransmitter::setPortRmtParams(rmt_tx_config, portIndex, memBlockNum, clkDiv);

    err = uninstallRmtDriver(rmt_tx_config.channel);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        xSemaphoreGive(transmitMutex_);
        ESP_LOGW("IRTransmitter", "Failed to uninstall RMT driver for port %d: %s", portIndex, esp_err_to_name(err));
    }

    // Configure the RMT channel
    err = rmt_config(&rmt_tx_config);
    if (err != ESP_OK) {
        xSemaphoreGive(transmitMutex_);
        ESP_LOGE("IRTransmitter", "Failed to configure RMT for port %d: %s", portIndex, esp_err_to_name(err));
        return err;
    }

    // Install the RMT driver
    err = rmt_driver_install(rmt_tx_config.channel, 0, 0);
    if (err != ESP_OK) {
        xSemaphoreGive(transmitMutex_);
        ESP_LOGE("IRTransmitter", "Failed to install RMT driver for port %d: %s", portIndex, esp_err_to_name(err));
        return err;
    }

    xSemaphoreGive(transmitMutex_);
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
