#ifdef EMBEDDED_BUILD
#include "IRTransmitter.h"
#include "configurationBackend.h"
#include <Arduino.h>
#include "utils.h"
#include "IRConstants.h"

// constructor
IRTransmitter::IRTransmitter(gpio_num_t gpio_num, rmt_channel_t channel, const IProtocolSettings* protocolSettings, const Encoder* encoder)
    : gpio_num_(gpio_num), channel_(channel), settings(protocolSettings), encoder(encoder) {

    static constexpr bool LOOP_ENABLE = false;
    static constexpr bool CARRIER_ENABLE = true;
    static constexpr bool OUTPUT_ENABLE = true;
    static constexpr rmt_idle_level_t IDLE_LEVEL = RMT_IDLE_LEVEL_LOW;
    static constexpr rmt_carrier_level_t CARRIER_LEVEL = RMT_CARRIER_LEVEL_HIGH;
    static constexpr int CARRIER_FREQ_HZ = 38000;
    static constexpr int CARRIER_DUTY_PERCENTAGE = 33;

    rmt_config_t rmt_tx_config;
    rmt_tx_config.rmt_mode = RMT_MODE_TX;
    rmt_tx_config.channel = channel_;
    rmt_tx_config.gpio_num = gpio_num_;
    rmt_tx_config.mem_block_num = IRConstants::MEM_BLOCK_NUM;
    rmt_tx_config.clk_div = IRConstants::CLK_DIV;
    rmt_tx_config.tx_config.loop_en = LOOP_ENABLE;
    rmt_tx_config.tx_config.carrier_en = CARRIER_ENABLE;
    rmt_tx_config.tx_config.idle_output_en = OUTPUT_ENABLE;
    rmt_tx_config.tx_config.idle_level = IDLE_LEVEL;
    rmt_tx_config.tx_config.carrier_level = CARRIER_LEVEL;
    rmt_tx_config.tx_config.carrier_freq_hz = CARRIER_FREQ_HZ;
    rmt_tx_config.tx_config.carrier_duty_percent = CARRIER_DUTY_PERCENTAGE;

    // Check if the RMT driver is already installed and uninstall it if necessary
    esp_err_t rmt_uninstall_res = rmt_driver_uninstall(rmt_tx_config.channel);
    if (rmt_uninstall_res == ESP_OK || rmt_uninstall_res == ESP_ERR_INVALID_STATE) {
        Utils::safeSerialPrintln("RMT driver uninstalled successfully or was not installed.");
    } else {
        Serial.printf("Failed to uninstall RMT driver: %d\n", rmt_uninstall_res);
    }

    // Configure RMT transmitter
    ESP_ERROR_CHECK(rmt_config(&rmt_tx_config));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_tx_config.channel, 0, 0));

    Utils::safeSerialPrintln("IR Transmitter initialized.");
}

// Destructor
IRTransmitter::~IRTransmitter() {
    // Uninstall the RMT driver
    ESP_ERROR_CHECK(rmt_driver_uninstall(channel_));
    Utils::safeSerialPrintln("IR Transmitter destroyed.");
}

void IRTransmitter::sendCommand(uint32_t address, uint32_t command) {
    rmt_item32_t items[settings->getFrameItemCount()];
    if (!encoder->buildPacket(items, address, command)){
        Serial.printf("incosistent encoder indexing\n");
    }

    // Send RMT items
    static constexpr int TIMEOUT_MS = 1000;
    ESP_ERROR_CHECK(rmt_write_items(channel_, items, settings->getFrameItemCount(), true));
    ESP_ERROR_CHECK(rmt_wait_tx_done(channel_, pdMS_TO_TICKS(TIMEOUT_MS))); // Wait for transmission to complete with a timeout of 1000ms
}

#endif // EMBEDDED_BUILD
