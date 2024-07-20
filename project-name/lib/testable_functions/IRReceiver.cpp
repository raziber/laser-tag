#ifdef EMBEDDED_BUILD

#include "IRReceiver.h"
#include "decoderHelperFunctions.h"

IRReceiver::IRReceiver(gpio_num_t gpio_num, rmt_channel_t channel)
    : gpio_num_(gpio_num), channel_(channel), rb_(nullptr) {}

void IRReceiver::init() {
    rmt_config_t rmt_rx_config;
    rmt_rx_config.rmt_mode = RMT_MODE_RX;
    rmt_rx_config.channel = channel_;
    rmt_rx_config.gpio_num = gpio_num_;
    rmt_rx_config.clk_div = rmtSettings::CLK_DIV;
    rmt_rx_config.mem_block_num = rmtSettings::MEM_BLOCK_NUM;
    rmt_rx_config.flags = rmtSettings::receiver::FLAGS;
    rmt_rx_config.rx_config.filter_en = rmtSettings::receiver::RECEIVER_FILTER_ENABLE;
    rmt_rx_config.rx_config.filter_ticks_thresh = rmtSettings::receiver::FILTER_TICKS_THRESH;
    rmt_rx_config.rx_config.idle_threshold = rmtSettings::receiver::IDLE_THRESHOLD;

    // Configure RMT receiver
    ESP_ERROR_CHECK(rmt_config(&rmt_rx_config));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_rx_config.channel, rmtSettings::RX_BUFFER_SIZE, 0));

    // Get ring buffer handle
    ESP_ERROR_CHECK(rmt_get_ringbuf_handle(channel_, &rb_));

    // Check if the ring buffer handle is valid
    if (rb_ == nullptr) {
        Serial.println("Failed to get ring buffer handle.");
        return;
    }

    // Start RMT receiver
    ESP_ERROR_CHECK(rmt_rx_start(channel_, true));

    // Create FreeRTOS task to handle received data
    if (xTaskCreate(receiveTask, "IR Receive Task", 2048, this, 10, nullptr) != pdPASS) {
        Serial.println("Failed to create FreeRTOS task");
    }
    Serial.println("IR Receiver initialized.");
}

void IRReceiver::receiveTask(void* param) {
    if (param == nullptr) {
        // Handle the error, e.g., log or throw an exception
        Serial.println("Error: param is null in receiveTask");
        return;
    }

    // IRReceiver* receiver = reinterpret_cast<IRReceiver*>(param);
    std::unique_ptr<IRReceiver> receiver(reinterpret_cast<IRReceiver*>(param));
    receiver->handleReceivedData();
}

void IRReceiver::handleReceivedData() {
    size_t rx_size = 0;
    rmt_item32_t* items = nullptr;
    while (true) {
        // Wait for data to be received
        items = static_cast<rmt_item32_t*>(xRingbufferReceive(rb_, &rx_size, portMAX_DELAY));
        if (items) {
            uint16_t address = 0, command = 0;
            if (rx_size == irProtocolSettings.frame_item_count * sizeof(rmt_item32_t)) {
                if (parseFrame(items, address, command)) {
                    Serial.printf("Channel: %d, Address: 0x%04X, Command: 0x%04X\n", channel_, address, command);
                } else {
                    Serial.println("Failed to parse frame.");
                }
            } else if (rx_size == irProtocolSettings.repeat_frame_item_count * sizeof(rmt_item32_t)) {
                if (parseRepeatFrame(items)) {
                    Serial.printf("Channel: %d, Address: 0x%04X, Command: 0x%04X (repeat)\n", channel_, address, command);
                } else {
                    Serial.println("Failed to parse repeat frame.");
                }
            } else {
                Serial.printf("Received incomplete frame, only %d items...\n", rx_size / sizeof(rmt_item32_t));
            }
            // Return the items to the ring buffer
            vRingbufferReturnItem(rb_, (void*)items);
        } else {
            Serial.println("No data received.");
        }
    }
}

#endif // EMBEDDED_BUILD
