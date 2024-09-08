#include "IRTransmitter.hpp"

IRTransmitter::IRTransmitter(const std::vector<int>& ports, std::unique_ptr<IREncoder> encoder, std::unique_ptr<IRProtocolSettings> protocolSettings, int memBlockNum, int clkDiv)
        : ports_(ports), encoder_(std::move(encoder)), protocolSettings_(std::move(protocolSettings)), memBlockNum_(memBlockNum), clkDiv_(clkDiv) {
    for (int port : ports_) {
        configurePort(port, memBlockNum, clkDiv);
    }
}

IRTransmitter::~IRTransmitter() {
    for (int port : ports_) {
        rmt_driver_uninstall(static_cast<rmt_channel_t>(port));
    }
}

void IRTransmitter::transmitToAllPorts(uint32_t address, uint32_t command) const {
    std::vector<rmt_item32_t> packet = encoder_->createPacket(address, command);

    for (int port : ports_) {
        ESP_ERROR_CHECK(rmt_write_items(static_cast<rmt_channel_t>(port), packet.data(), packet.size(), true));
        static constexpr int TIMEOUT_MS = 1000;
        ESP_ERROR_CHECK(rmt_wait_tx_done(static_cast<rmt_channel_t>(port), pdMS_TO_TICKS(TIMEOUT_MS)));
    }
}

void IRTransmitter::transmitToSinglePort(uint32_t address, uint32_t command, uint32_t portId) const {
    std::vector<rmt_item32_t> packet = encoder_->createPacket(address, command);

    ESP_ERROR_CHECK(rmt_write_items(static_cast<rmt_channel_t>(portId), packet.data(), packet.size(), true));
    static constexpr int TIMEOUT_MS = 1000;
    ESP_ERROR_CHECK(rmt_wait_tx_done(static_cast<rmt_channel_t>(portId), pdMS_TO_TICKS(TIMEOUT_MS)));
}

void IRTransmitter::configurePort(int port, int memBlockNum, int clkDiv){
    static constexpr bool LOOP_ENABLE = false;
    static constexpr bool CARRIER_ENABLE = true;
    static constexpr bool OUTPUT_ENABLE = true;
    static constexpr rmt_idle_level_t IDLE_LEVEL = RMT_IDLE_LEVEL_LOW;
    static constexpr rmt_carrier_level_t CARRIER_LEVEL = RMT_CARRIER_LEVEL_HIGH;
    static constexpr int CARRIER_FREQ_HZ = 38000;
    static constexpr int CARRIER_DUTY_PERCENTAGE = 33;

    rmt_config_t rmt_tx_config;
    rmt_tx_config.rmt_mode = RMT_MODE_TX;
    rmt_tx_config.channel = port;           // basically ID from 0 to 7
    rmt_tx_config.gpio_num = gpio[port];         // physical port to connect TODO: fix this
    rmt_tx_config.mem_block_num = memBlockNum;
    rmt_tx_config.clk_div = clkDiv;
    rmt_tx_config.tx_config.loop_en = LOOP_ENABLE;
    rmt_tx_config.tx_config.carrier_en = CARRIER_ENABLE;
    rmt_tx_config.tx_config.idle_output_en = OUTPUT_ENABLE;
    rmt_tx_config.tx_config.idle_level = IDLE_LEVEL;
    rmt_tx_config.tx_config.carrier_level = CARRIER_LEVEL;
    rmt_tx_config.tx_config.carrier_freq_hz = CARRIER_FREQ_HZ;
    rmt_tx_config.tx_config.carrier_duty_percent = CARRIER_DUTY_PERCENTAGE;

    manageRmtDriver(rmt_tx_config.channel);
    ESP_ERROR_CHECK(rmt_config(&rmt_tx_config));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_tx_config.channel, 0, 0));
}

void IRTransmitter::manageRmtDriver(rmt_channel_t channel){
    // Check if the RMT driver is already installed and uninstall it if necessary
    esp_err_t rmt_uninstall_res = rmt_driver_uninstall(channel);
    if (rmt_uninstall_res == ESP_OK || rmt_uninstall_res == ESP_ERR_INVALID_STATE) {
        Serial.println("RMT driver uninstalled successfully or was not installed.");
    } else {
        Serial.printf("Failed to uninstall RMT driver: %d\n", rmt_uninstall_res);
    }
}