#ifdef EMBEDDED_BUILD
#include "IRTransmitter.h"
#include "encoderHelperFunctions.h"

IRTransmitter::IRTransmitter(gpio_num_t gpio_num, rmt_channel_t channel)
    : gpio_num_(gpio_num), channel_(channel) {}

void IRTransmitter::init() {
    rmt_config_t rmt_tx_config;
    rmt_tx_config.rmt_mode = RMT_MODE_TX;
    rmt_tx_config.channel = channel_;
    rmt_tx_config.gpio_num = gpio_num_;
    rmt_tx_config.mem_block_num = 1;
    rmt_tx_config.clk_div = RMT_CLK_DIV;
    rmt_tx_config.tx_config.loop_en = false;
    rmt_tx_config.tx_config.carrier_en = true;
    rmt_tx_config.tx_config.idle_output_en = true;
    rmt_tx_config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    rmt_tx_config.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
    rmt_tx_config.tx_config.carrier_freq_hz = 38000; // 38kHz carrier frequency
    rmt_tx_config.tx_config.carrier_duty_percent = 33;

    // Configure RMT transmitter
    ESP_ERROR_CHECK(rmt_config(&rmt_tx_config));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_tx_config.channel, 0, 0));

    Serial.println("IR Transmitter initialized.");
}

void IRTransmitter::sendCommand(uint32_t address, uint32_t command) {
    rmt_item32_t items[FRAME_ITEM_COUNT];
    if (!buildPacket(items, address, command)){
        Serial.printf("incosistent encoder indexing\n");
    }

    // Send RMT items
    ESP_ERROR_CHECK(rmt_write_items(channel_, items, FRAME_ITEM_COUNT, true));
    ESP_ERROR_CHECK(rmt_wait_tx_done(channel_, pdMS_TO_TICKS(1000))); // Wait for transmission to complete with a timeout of 1000ms
}

#endif // EMBEDDED_BUILD
