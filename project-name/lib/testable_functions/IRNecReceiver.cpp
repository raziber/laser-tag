#ifdef EMBEDDED_BUILD
#include "IRNecReceiver.h"

IRNecReceiver::IRNecReceiver(gpio_num_t gpio_num, rmt_channel_t channel)
    : gpio_num_(gpio_num), channel_(channel), rb_(nullptr) {}

void IRNecReceiver::init() {
    rmt_config_t rmt_rx_config;
    rmt_rx_config.rmt_mode = RMT_MODE_RX;
    rmt_rx_config.channel = channel_;
    rmt_rx_config.gpio_num = gpio_num_;
    rmt_rx_config.clk_div = 80;
    rmt_rx_config.mem_block_num = 1;
    rmt_rx_config.flags = 0;
    rmt_rx_config.rx_config.filter_en = true;
    rmt_rx_config.rx_config.filter_ticks_thresh = 100;
    rmt_rx_config.rx_config.idle_threshold = 12000;

    // Configure RMT receiver
    ESP_ERROR_CHECK(rmt_config(&rmt_rx_config));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_rx_config.channel, 1000, 0));

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

bool IRNecReceiver::checkInRange(uint32_t duration, uint32_t spec) {
    return (duration < (spec + NEC_DECODE_MARGIN)) && (duration > (spec - NEC_DECODE_MARGIN));
}

bool IRNecReceiver::parseLogic0(rmt_item32_t* item) {
    return checkInRange(item->duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
           checkInRange(item->duration1, NEC_PAYLOAD_ZERO_DURATION_1);
}

bool IRNecReceiver::parseLogic1(rmt_item32_t* item) {
    return checkInRange(item->duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
           checkInRange(item->duration1, NEC_PAYLOAD_ONE_DURATION_1);
}

bool IRNecReceiver::parseFrame(rmt_item32_t* items, uint16_t& address, uint16_t& command) {
    if (!checkInRange(items->duration0, NEC_LEADING_CODE_DURATION_0) ||
        !checkInRange(items->duration1, NEC_LEADING_CODE_DURATION_1)) {
        return false;
    }
    items++;
    for (int i = 0; i < 16; i++) {
        if (parseLogic1(items)) {
            address |= 1 << i;
        } else if (parseLogic0(items)) {
            address &= ~(1 << i);
        } else {
            return false;
        }
        items++;
    }
    for (int i = 0; i < 16; i++) {
        if (parseLogic1(items)) {
            command |= 1 << i;
        } else if (parseLogic0(items)) {
            command &= ~(1 << i);
        } else {
            return false;
        }
        items++;
    }
    return true;
}

bool IRNecReceiver::parseRepeatFrame(rmt_item32_t* items) {
    return checkInRange(items->duration0, NEC_REPEAT_CODE_DURATION_0) &&
           checkInRange(items->duration1, NEC_REPEAT_CODE_DURATION_1);
}

void IRNecReceiver::receiveTask(void* param) {
    IRNecReceiver* receiver = static_cast<IRNecReceiver*>(param);
    receiver->handleReceivedData();
}

void IRNecReceiver::handleReceivedData() {
    size_t rx_size = 0;
    rmt_item32_t* items = nullptr;
    while (true) {
        // Wait for data to be received
        items = (rmt_item32_t*) xRingbufferReceive(rb_, &rx_size, portMAX_DELAY);
        if (items) {
            Serial.printf("Data received: %d items\n", rx_size / sizeof(rmt_item32_t));
            uint16_t address = 0, command = 0;
            if (rx_size == 34 * sizeof(rmt_item32_t)) {
                if (parseFrame(items, address, command)) {
                    Serial.printf("Address: 0x%04X, Command: 0x%04X\n", address, command);
                } else {
                    Serial.println("Failed to parse frame.");
                }
            } else if (rx_size == 2 * sizeof(rmt_item32_t)) {
                if (parseRepeatFrame(items)) {
                    Serial.printf("Address: 0x%04X, Command: 0x%04X (repeat)\n", address, command);
                } else {
                    Serial.println("Failed to parse repeat frame.");
                }
            } else {
                Serial.println("Received incomplete frame.");
            }
            // Return the items to the ring buffer
            vRingbufferReturnItem(rb_, (void*) items);
        } else {
            Serial.println("No data received.");
        }
    }
}

#endif // EMBEDDED_BUILD
