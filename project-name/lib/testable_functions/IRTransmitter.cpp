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
    rmt_tx_config.mem_block_num = rmtSettings::MEM_BLOCK_NUM;
    rmt_tx_config.clk_div = rmtSettings::CLK_DIV;
    rmt_tx_config.tx_config.loop_en = rmtSettings::trasmitter::LOOP_ENABLE;
    rmt_tx_config.tx_config.carrier_en = rmtSettings::trasmitter::CARRIER_ENABLE;
    rmt_tx_config.tx_config.idle_output_en = rmtSettings::trasmitter::OUTPUT_ENABLE;
    rmt_tx_config.tx_config.idle_level = rmtSettings::trasmitter::IDLE_LEVEL;
    rmt_tx_config.tx_config.carrier_level = rmtSettings::trasmitter::CARRIER_LEVEL;
    rmt_tx_config.tx_config.carrier_freq_hz = rmtSettings::trasmitter::CARRIER_FREQ_HZ;
    rmt_tx_config.tx_config.carrier_duty_percent = rmtSettings::trasmitter::CARRIER_DUTY_PERCENTAGE;

    // Configure RMT transmitter
    ESP_ERROR_CHECK(rmt_config(&rmt_tx_config));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_tx_config.channel, 0, 0));

    Serial.println("IR Transmitter initialized.");
}

void IRTransmitter::sendCommand(uint32_t address, uint32_t command) {
    rmt_item32_t items[irProtocolSettings.frame_item_count];
    if (!buildPacket(items, address, command)){
        Serial.printf("incosistent encoder indexing\n");
    }

    // Send RMT items
    ESP_ERROR_CHECK(rmt_write_items(channel_, items, irProtocolSettings.frame_item_count, true));
    ESP_ERROR_CHECK(rmt_wait_tx_done(channel_, pdMS_TO_TICKS(rmtSettings::trasmitter::TIMEOUT_MS))); // Wait for transmission to complete with a timeout of 1000ms
}

#endif // EMBEDDED_BUILD
