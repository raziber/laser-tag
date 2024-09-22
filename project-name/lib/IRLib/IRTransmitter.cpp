#include "IRTransmitter.hpp"

IRTransmitter::IRTransmitter(const std::vector<int>& ports, std::unique_ptr<IREncoder> encoder, int memBlockNum, int clkDiv)
    : gpioPorts_(ports), encoder_(std::move(encoder)), memBlockNum_(memBlockNum), clkDiv_(clkDiv), taskHandle_(nullptr) {

    transmitMutex_ = xSemaphoreCreateMutex();
    if (transmitMutex_ == nullptr) {
        ESP_LOGE("IRTransmitter", "Failed to create mutex");
        return;
    }

    transmissionQueue_ = xQueueCreate(10, sizeof(TransmissionRequest));
    if (transmissionQueue_ == nullptr) {
        ESP_LOGE("IRTransmitter", "Failed to create transmission queue");
        vSemaphoreDelete(transmitMutex_);
        transmitMutex_ = nullptr;
        return;
    }

    for (std::size_t i = 0; i < gpioPorts_.size(); ++i) {
        esp_err_t err = configurePort(i, memBlockNum_, clkDiv_);
        if (err != ESP_OK) {
            ESP_LOGE("IRTransmitter", "Failed to configure port %d: %s", i, esp_err_to_name(err));
            // Continue configuring other ports
        }
    }

    // Create the transmitter task
    BaseType_t result = xTaskCreate(
        IRTransmitter::transmitterTask,
        "IRTransmitterTask",
        4096,    // Stack size
        this,    // Task parameter
        5,       // Task priority
        &taskHandle_
    );

    if (result != pdPASS) {
        ESP_LOGE("IRTransmitter", "Failed to create transmitter task");
        taskHandle_ = nullptr;
        // Clean up resources
        vSemaphoreDelete(transmitMutex_);
        transmitMutex_ = nullptr;
        vQueueDelete(transmissionQueue_);
        transmissionQueue_ = nullptr;
    }
}

IRTransmitter::~IRTransmitter() {
    if (taskHandle_ != nullptr) {
        vTaskDelete(taskHandle_);
        taskHandle_ = nullptr;
    }

    if (transmissionQueue_ != nullptr) {
        vQueueDelete(transmissionQueue_);
        transmissionQueue_ = nullptr;
    }

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

esp_err_t IRTransmitter::transmitToAllPorts(uint32_t address, uint32_t command) {
    if (transmissionQueue_ == nullptr) {
        ESP_LOGE("IRTransmitter", "Transmission queue not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    TransmissionRequest request;
    request.address = address;
    request.command = command;
    request.portIndex = UINT32_MAX;  // Indicates all ports

    if (xQueueSend(transmissionQueue_, &request, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE("IRTransmitter", "Failed to enqueue transmission request");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t IRTransmitter::transmitToSinglePort(uint32_t portIndex, uint32_t address, uint32_t command) {
    if (transmissionQueue_ == nullptr) {
        ESP_LOGE("IRTransmitter", "Transmission queue not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = validatePortIndex(portIndex);
    if (err != ESP_OK) {
        return err;
    }

    TransmissionRequest request;
    request.address = address;
    request.command = command;
    request.portIndex = portIndex;

    if (xQueueSend(transmissionQueue_, &request, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE("IRTransmitter", "Failed to enqueue transmission request");
        return ESP_FAIL;
    }

    return ESP_OK;
}

void IRTransmitter::transmitterTask(void* arg) {
    IRTransmitter* transmitter = static_cast<IRTransmitter*>(arg);

    while (true) {
        TransmissionRequest request;
        if (xQueueReceive(transmitter->transmissionQueue_, &request, portMAX_DELAY) == pdTRUE) {
            std::vector<rmt_item32_t> packet;
            esp_err_t err = transmitter->createPacket(packet, request.address, request.command);
            if (err != ESP_OK) {
                ESP_LOGE("IRTransmitter", "Failed to create packet: %s", esp_err_to_name(err));
                continue;
            }

            if (request.portIndex == UINT32_MAX) {
                // Transmit to all ports
                for (size_t i = 0; i < transmitter->gpioPorts_.size(); ++i) {
                    err = transmitter->transmitToPort(i, packet);
                    if (err != ESP_OK) {
                        ESP_LOGE("IRTransmitter", "Failed to transmit to port %d: %s", i, esp_err_to_name(err));
                        // Optionally handle retries or errors
                    }
                }
            } else {
                // Transmit to single port
                err = transmitter->transmitToPort(request.portIndex, packet);
                if (err != ESP_OK) {
                    ESP_LOGE("IRTransmitter", "Failed to transmit to port %d: %s", request.portIndex, esp_err_to_name(err));
                    // Optionally handle retries or errors
                }
            }
        }
    }
}

esp_err_t IRTransmitter::createPacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command) {
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

esp_err_t IRTransmitter::transmitToPort(uint32_t portIndex, std::vector<rmt_item32_t>& packet) {
    esp_err_t err = validatePortIndex(portIndex);
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

esp_err_t IRTransmitter::validatePortIndex(uint32_t portIndex) {
    if (portIndex >= gpioPorts_.size() || portIndex >= RMT_CHANNEL_MAX) {
        ESP_LOGE("IRTransmitter", "Invalid port index: %d", portIndex);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

void IRTransmitter::setPortRmtParams(rmt_config_t& rmt_tx_config, int portIndex, int memBlockNum, int clkDiv) {
    rmt_tx_config.rmt_mode = RMT_MODE_TX;
    rmt_tx_config.channel = static_cast<rmt_channel_t>(portIndex);
    rmt_tx_config.gpio_num = static_cast<gpio_num_t>(gpioPorts_[portIndex]);
    rmt_tx_config.mem_block_num = memBlockNum;
    rmt_tx_config.clk_div = clkDiv;
    rmt_tx_config.tx_config.loop_en = false;
    rmt_tx_config.tx_config.carrier_en = false;
    rmt_tx_config.tx_config.idle_output_en = true;
    rmt_tx_config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
}

esp_err_t IRTransmitter::configurePort(int portIndex, int memBlockNum, int clkDiv) {
    esp_err_t err = validatePortIndex(portIndex);
    if (err != ESP_OK) {
        return err;
    }

    rmt_config_t rmt_tx_config = {};
    setPortRmtParams(rmt_tx_config, portIndex, memBlockNum, clkDiv);

    err = uninstallRmtDriver(rmt_tx_config.channel);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGW("IRTransmitter", "Failed to uninstall RMT driver for port %d: %s", portIndex, esp_err_to_name(err));
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
        return ESP_OK;  // Return OK since the driver wasn't installed
    } else {
        ESP_LOGE("IRTransmitter", "Failed to uninstall RMT driver for channel %d: %s", channel, esp_err_to_name(err));
        return err;  // Return the actual error for the caller to handle
    }

    return ESP_OK;  // Return OK if uninstalled successfully
}
