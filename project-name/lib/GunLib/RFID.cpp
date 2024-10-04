// RFID.cpp
#include "RFID.hpp"
#include "RFIDConfig.hpp"
#include "MFRC522Commands.hpp"
#include <esp_log.h>
#include "Macros.hpp"
#include <string>

RFID::RFID(SPIBus& spiBus, int csPin)
    : spiDevice_(spiBus.addDeviceToBus(csPin, RFIDConfig::spiClockSpeedHz)) {
    if (!initResetPin()) {
        throw std::runtime_error("Failed to initialize reset pin");
    }
}

void RFID::startMonitoring(Queue<std::string>& rfidQueue) {
    // Create the task
    try {
        readingTask_ = std::make_shared<Task>(
            "RfidReadingTask",
            4096,   // Stack size in bytes
            1,      // Priority
            [this, &rfidQueue]() { updateRead(rfidQueue); }
        );
    } catch (const std::runtime_error& e) {
        ESP_LOGE("RFID", "Failed to create RFID reader reading task: %s", e.what());
    }
}

void RFID::updateRead(Queue<std::string>& rfidQueue) {
    while (true) {
        // Simulate reading data from the RFID reader
        std::string playerID = "SamplePlayerID";  // Placeholder for actual RFID read logic
        if (!rfidQueue.send(&playerID)) {
            ESP_LOGE("RFID", "Failed to send player ID to queue: Queue might be full");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

std::optional<uint8_t> RFID::readRegister(Register reg) {
    uint8_t address = ((static_cast<uint8_t>(reg) << 1) & 0x7E) | 0x80;
    uint8_t data[2] = { address, 0 };

    spi_transaction_t transaction = {};
    transaction.length = 8 * sizeof(data);
    transaction.tx_buffer = data;
    transaction.rx_buffer = data;

    if (!spiDevice_.transmit(&transaction)) {
        ESP_LOGE("RFID", "Failed to read reg %d", reg);
        return std::nullopt;
    }

    return data[1];
}

bool RFID::writeRegister(Register reg, uint8_t value) {
    uint8_t address = (static_cast<uint8_t>(reg) << 1) & 0x7E;
    uint8_t data[2] = { address, value };

    spi_transaction_t transaction = {};
    transaction.length = 8 * sizeof(data);
    transaction.tx_buffer = data;

    if (!spiDevice_.transmit(&transaction)) {
        ESP_LOGE("RFID", "Failed to write reg %d, with value %d", reg, value);
        return false;
    }

    return true;
}

bool RFID::initResetPin() {
    if (gpio_set_direction(RFIDConfig::rstPin, GPIO_MODE_OUTPUT) != ESP_OK) {
        return false;
    }
    return true;
}

bool RFID::reset() {
    gpio_num_t rstPin = RFIDConfig::rstPin;

    if (gpio_set_level(RFIDConfig::rstPin, 0) != ESP_OK) {
        throw std::runtime_error("Failed to set reset pin LOW");
    }
    vTaskDelay(pdMS_TO_TICKS(50));
    if (gpio_set_level(RFIDConfig::rstPin, 1) != ESP_OK) {
        throw std::runtime_error("Failed to set reset pin HIGH");
    }
    vTaskDelay(pdMS_TO_TICKS(50));
    if (!writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::SoftReset))) {
        throw std::runtime_error("Failed to reset RFID reader");
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    return true;
}