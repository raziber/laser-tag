#include "IRReceiver.hpp"

IRReceiver::IRReceiver(IRProtocol protocol, gpio_num_t gpioNum, int memBlockNum, int clkDiv)
    : protocol_(protocol), gpioNum_(gpioNum), memBlockNum_(memBlockNum), clkDiv_(clkDiv), channel_(RMT_CHANNEL_0), taskHandle_(nullptr), dataQueue_(nullptr) {

    receiverMutex_ = xSemaphoreCreateMutex();
    if (receiverMutex_ == nullptr) {
        ESP_LOGE("IRReceiver", "Failed to create mutex");
        return;
    }

    // Initialize protocol settings
    protocolSettings_ = IRProtocolFactory::createProtocolSettings(protocol_);
    if (!protocolSettings_) {
        ESP_LOGE("IRReceiver", "Failed to create protocol settings.");
        return;
    }

    // Initialize RMT channel
    esp_err_t err = configureRMT();
    if (err != ESP_OK) {
        ESP_LOGE("IRReceiver", "Failed to configure RMT: %s", esp_err_to_name(err));
    }
}

IRReceiver::~IRReceiver() {
    stop();  // Ensure the task is stopped before destructing

    esp_err_t err = uninstallRMT();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGW("IRReceiver", "Failed to uninstall RMT driver: %s", esp_err_to_name(err));
    }

    if (receiverMutex_ != nullptr) {
        vSemaphoreDelete(receiverMutex_);
        receiverMutex_ = nullptr;
    }
}

void IRReceiver::setDataQueue(QueueHandle_t queue) {
    dataQueue_ = queue;
}

esp_err_t IRReceiver::configureRMT() {
    if (!protocolSettings_) {
        ESP_LOGE("IRReceiver", "Protocol settings not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    rmt_config_t rmt_rx_config = {};
    rmt_rx_config.rmt_mode = RMT_MODE_RX;
    rmt_rx_config.channel = channel_;
    rmt_rx_config.gpio_num = gpioNum_;
    rmt_rx_config.mem_block_num = memBlockNum_;
    rmt_rx_config.clk_div = clkDiv_;
    rmt_rx_config.rx_config.filter_en = true;
    rmt_rx_config.rx_config.filter_ticks_thresh = protocolSettings_->getFilterTicksThreshold();
    rmt_rx_config.rx_config.idle_threshold = protocolSettings_->getIdleThreshold();

    esp_err_t err = rmt_config(&rmt_rx_config);
    if (err != ESP_OK) {
        ESP_LOGE("IRReceiver", "Failed to configure RMT: %s", esp_err_to_name(err));
        return err;
    }

    err = rmt_driver_install(channel_, 1000, 0);
    if (err != ESP_OK) {
        ESP_LOGE("IRReceiver", "Failed to install RMT driver: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

esp_err_t IRReceiver::uninstallRMT() {
    esp_err_t err = rmt_driver_uninstall(channel_);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE("IRReceiver", "Failed to uninstall RMT driver: %s", esp_err_to_name(err));
        return err;
    }
    return ESP_OK;
}

esp_err_t IRReceiver::start() {
    if (receiverMutex_ == nullptr) {
        ESP_LOGE("IRReceiver", "Receiver mutex not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(receiverMutex_, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE("IRReceiver", "Failed to take mutex");
        return ESP_FAIL;
    }

    if (taskHandle_ != nullptr) {
        ESP_LOGW("IRReceiver", "Receiver task already running");
        xSemaphoreGive(receiverMutex_);
        return ESP_OK;
    }

    // Start the receiver task
    BaseType_t result = xTaskCreate(
        IRReceiver::receiverTask,
        "IRReceiverTask",
        4096,    // Stack size
        this,    // Task parameter
        5,       // Task priority
        &taskHandle_
    );

    if (result != pdPASS) {
        ESP_LOGE("IRReceiver", "Failed to create receiver task");
        taskHandle_ = nullptr;
        xSemaphoreGive(receiverMutex_);
        return ESP_FAIL;
    }

    xSemaphoreGive(receiverMutex_);
    return ESP_OK;
}

esp_err_t IRReceiver::stop() {
    if (receiverMutex_ == nullptr) {
        ESP_LOGE("IRReceiver", "Receiver mutex not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(receiverMutex_, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE("IRReceiver", "Failed to take mutex");
        return ESP_FAIL;
    }

    if (taskHandle_ != nullptr) {
        vTaskDelete(taskHandle_);
        taskHandle_ = nullptr;
    }

    xSemaphoreGive(receiverMutex_);
    return ESP_OK;
}

void IRReceiver::receiverTask(void* arg) {
    IRReceiver* receiver = static_cast<IRReceiver*>(arg);

    // Start RMT reception
    rmt_rx_start(receiver->channel_, true);

    RingbufHandle_t rb = NULL;
    esp_err_t err = rmt_get_ringbuf_handle(receiver->channel_, &rb);
    if (err != ESP_OK) {
        ESP_LOGE("IRReceiver", "Failed to get ring buffer handle: %s", esp_err_to_name(err));
        vTaskDelete(NULL);  // Exit task
    }

    while (true) {
        receiver->receiveAndProcessData(rb);
    }
}

void IRReceiver::receiveAndProcessData(RingbufHandle_t rb) {
    size_t rx_size = 0;
    rmt_item32_t* items = (rmt_item32_t*)xRingbufferReceive(rb, &rx_size, portMAX_DELAY);
    if (items) {
        size_t num_items = rx_size / sizeof(rmt_item32_t);

        std::vector<rmt_item32_t> rawData(items, items + num_items);

        processReceivedItems(rawData);

        // Return the memory to the ring buffer
        vRingbufferReturnItem(rb, (void*)items);
    } else {
        // Handle ring buffer receive failure
        ESP_LOGE("IRReceiver", "Failed to receive items from ring buffer");
        vTaskDelay(pdMS_TO_TICKS(100));  // Delay before retrying
    }
}

void IRReceiver::processReceivedItems(const std::vector<rmt_item32_t>& rawData) {
    uint32_t address = 0;
    uint32_t command = 0;

    if (!protocolSettings_) {
        ESP_LOGE("IRReceiver", "Protocol settings not initialized.");
        return;
    }

    IRDecoder decoder(std::make_unique<IRProtocolSettings>(*protocolSettings_));

    esp_err_t err = decoder.decode(rawData, address, command);
    if (err == ESP_OK) {
        handleDecodedData(address, command);
    } else {
        ESP_LOGE("IRReceiver", "Failed to decode data: %s", esp_err_to_name(err));
    }
}

void IRReceiver::handleDecodedData(uint32_t address, uint32_t command) {
    // Successfully decoded
    ESP_LOGI("IRReceiver", "Received address: 0x%X, command: 0x%X", address, command);

    // If a data queue is set, send the decoded data
    if (dataQueue_ != nullptr) {
        uint64_t data = ((uint64_t)address << 32) | command;
        xQueueSend(dataQueue_, &data, portMAX_DELAY);
    }
}
