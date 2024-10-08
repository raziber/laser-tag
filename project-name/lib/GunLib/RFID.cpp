// RFID.cpp
#include "RFID.hpp"
#include "RFIDConfig.hpp"
#include "MFRC522Commands.hpp"
#include <esp_log.h>
#include "Macros.hpp"
#include <string>
#include "SPIBus.hpp"
#include "MutexLockGuard.hpp"
#include <sstream>
#include <iomanip>

RFID::RFID(SPIBus& spiBus, int csPin)
    : spiDevice_(spiBus.addDeviceToBus(csPin, RFIDConfig::spiClockSpeedHz)) {
    rfidMutex_ = xSemaphoreCreateRecursiveMutex();
    if (rfidMutex_ == nullptr) {
        throw std::runtime_error("Failed to create RFID mutex");
    }
    
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
    while (!readingTask_->shouldStop()) {
        std::string playerID = readPlayerID();

        if (!playerID.empty()) {
            if (!rfidQueue.send(&playerID)) {
                ESP_LOGE("RFID", "Failed to send player ID to queue: Queue might be full");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(500));  // Adjust delay as needed
    }
}

bool RFID::antennaOn() {
    MutexLockGuard lock(rfidMutex_);

    uint8_t txControlReg = readRegister(Register::TxControlRegister).value_or(0);
    if (!(txControlReg & 0x03)) {
        return writeRegister(Register::TxControlRegister, txControlReg | 0x03);
    }
    return true;
}

bool RFID::request(uint8_t requestMode, std::vector<uint8_t>& atqa) {
    MutexLockGuard lock(rfidMutex_);

    // Set BitFramingRegister to start sending at the first bit
    if (!writeRegister(Register::BitFramingRegister, 0x07)) {
        ESP_LOGE("RFID", "Failed to set BitFramingRegister");
        return false;
    }

    std::vector<uint8_t> sendData = { requestMode };

    if (!communicateWithPICC(Command::Transceive, sendData, atqa)) {
        ESP_LOGE("RFID", "Failed to communicate with PICC");
        return false;
    }

    if (atqa.size() != 2) {
        ESP_LOGE("RFID", "Invalid ATQA size: %d", atqa.size());
        return false;
    }

    return true;
}

bool RFID::communicateWithPICC(Command command, const std::vector<uint8_t>& sendData, std::vector<uint8_t>& backData) {
    MutexLockGuard lock(rfidMutex_);

    // Clear all interrupts
    if (!writeRegister(Register::CommIrqReg, 0x7F)) {
        ESP_LOGE("RFID", "Failed to clear interrupt flags");
        return false;
    }

    // Flush FIFO buffer
    if (!writeRegister(Register::FIFOLevelRegister, 0x80)) {
        ESP_LOGE("RFID", "Failed to flush FIFO buffer");
        return false;
    }

    // Write data to FIFO
    for (uint8_t byte : sendData) {
        if (!writeRegister(Register::FIFODataRegister, byte)) {
            ESP_LOGE("RFID", "Failed to write to FIFO");
            return false;
        }
    }

    // Start the command
    if (!writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::Idle))) {
        ESP_LOGE("RFID", "Failed to set CommandRegister to Idle");
        return false;
    }
    if (!writeRegister(Register::CommandRegister, static_cast<uint8_t>(command))) {
        ESP_LOGE("RFID", "Failed to start command");
        return false;
    }

    // Set bit framing if necessary (reset to 0)
    if (!writeRegister(Register::BitFramingRegister, 0x00)) {
        ESP_LOGE("RFID", "Failed to set BitFramingRegister");
        return false;
    }

    // Wait for the command to complete
    const uint8_t waitIRq = 0x30;  // RxIRq and IdleIRq
    for (int i = 0; i < 200; ++i) {
        uint8_t irqReg = readRegister(Register::CommIrqReg).value_or(0);
        if (irqReg & waitIRq) {
            break;  // Command completed
        }
        vTaskDelay(pdMS_TO_TICKS(1));  // Delay 1ms
    }

    // Check for errors
    uint8_t errorReg = readRegister(Register::ErrorReg).value_or(0);
    if (errorReg & 0x1B) {  // Check for buffer overflow, parity error, protocol error
        ESP_LOGE("RFID", "Communication error: 0x%02X", errorReg);
        return false;
    }

    // Read data from FIFO
    uint8_t fifoLevel = readRegister(Register::FIFOLevelRegister).value_or(0);
    if (fifoLevel == 0) {
        ESP_LOGE("RFID", "No data in FIFO");
        return false;
    }

    for (uint8_t i = 0; i < fifoLevel; ++i) {
        uint8_t byte = readRegister(Register::FIFODataRegister).value_or(0);
        backData.push_back(byte);
    }

    return true;
}

bool RFID::selectTag(std::vector<uint8_t>& uid) {
    MutexLockGuard lock(rfidMutex_);

    std::vector<uint8_t> buffer;
    std::vector<uint8_t> uidComplete;

    // Anti-collision command for cascade level 1
    buffer = { static_cast<uint8_t>(PICCCommand::AntiCollisionCL1), 0x20 };

    if (!communicateWithPICC(Command::Transceive, buffer, uidComplete)) {
        ESP_LOGE("RFID", "Failed during anti-collision");
        return false;
    }
    
    if (uidComplete.size() != 5) {
        ESP_LOGE("RFID", "Invalid UID size during anti-collision: %d", uidComplete.size());
        return false;
    }

    // Calculate BCC (Block Check Character)
    uint8_t bcc = uidComplete[4];
    uint8_t calculatedBCC = uidComplete[0] ^ uidComplete[1] ^ uidComplete[2] ^ uidComplete[3];
    if (bcc != calculatedBCC) {
        ESP_LOGE("RFID", "BCC mismatch");
        return false;
    }

    // UID bytes are uidComplete[0..3]
    uid.assign(uidComplete.begin(), uidComplete.begin() + 4);

    // Select the tag
    buffer.clear();
    buffer.push_back(0x93);  // Cascade Level 1
    buffer.push_back(0x70);  // Select command
    buffer.insert(buffer.end(), uid.begin(), uid.end());  // UID
    buffer.push_back(bcc);  // BCC

    // Calculate CRC_A
    std::vector<uint8_t> crc;
    if (!calculateCRC(buffer, crc)) {
        ESP_LOGE("RFID", "Failed to calculate CRC");
        return false;
    }
    buffer.insert(buffer.end(), crc.begin(), crc.end());

    std::vector<uint8_t> sak;  // Select Acknowledge
    if (!communicateWithPICC(Command::Transceive, buffer, sak)) {
        ESP_LOGE("RFID", "Failed during tag selection");
        return false;
    }

    if (sak.empty() || (sak[0] & 0x04)) {
        ESP_LOGE("RFID", "No SAK received or UID not complete");
        return false;
    }

    // UID successfully read and tag selected
    return true;
}

bool RFID::calculateCRC(const std::vector<uint8_t>& data, std::vector<uint8_t>& result) {
    MutexLockGuard lock(rfidMutex_);

    // Clear the CRC result registers
    if (!writeRegister(Register::DivIrqReg, 0x04)) {
        ESP_LOGE("RFID", "Failed to clear DivIrqReg");
        return false;
    }
    if (!writeRegister(Register::FIFOLevelRegister, 0x80)) {
        ESP_LOGE("RFID", "Failed to flush FIFO buffer");
        return false;
    }

    // Write data to FIFO
    for (uint8_t byte : data) {
        if (!writeRegister(Register::FIFODataRegister, byte)) {
            ESP_LOGE("RFID", "Failed to write to FIFO");
            return false;
        }
    }

    // Start the CRC calculation
    if (!writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::CalculateCRC))) {
        ESP_LOGE("RFID", "Failed to start CRC calculation");
        return false;
    }

    // Wait for CRC calculation to complete
    const int maxCycles = 5000;
    for (int i = 0; i < maxCycles; ++i) {
        uint8_t irqReg = readRegister(Register::DivIrqReg).value_or(0);
        if (irqReg & 0x04) {
            break;  // CRC calculation completed
        }
    }

    // Read CRC result
    uint8_t crcA = readRegister(Register::CRCResultLowRegister).value_or(0);
    uint8_t crcB = readRegister(Register::CRCResultHighRegister).value_or(0);

    result = { crcA, crcB };
    return true;
}

std::string RFID::readPlayerID() {
    MutexLockGuard lock(rfidMutex_);

    // Ensure the antenna is on
    if (!antennaOn()) {
        ESP_LOGE("RFID", "Failed to turn on antenna");
        return "";
    }

    // Send a Request command to search for tags
    std::vector<uint8_t> atqa;
    if (!request(static_cast<uint8_t>(PICCCommand::RequestA), atqa)) {
        // No tag found
        return "";
    }

    // Select the tag to retrieve its UID
    std::vector<uint8_t> uid;
    if (!selectTag(uid)) {
        ESP_LOGE("RFID", "Failed to select tag");
        return "";
    }

    // Convert UID bytes to a hexadecimal string
    std::stringstream uidStream;
    uidStream << std::hex << std::setfill('0');
    for (uint8_t byte : uid) {
        uidStream << std::setw(2) << static_cast<int>(byte);
    }

    std::string playerID = uidStream.str();
    ESP_LOGI("RFID", "Read Player ID: %s", playerID.c_str());

    return playerID;
}

std::optional<uint8_t> RFID::readRegister(Register reg) {
    MutexLockGuard lock(rfidMutex_);

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
    MutexLockGuard lock(rfidMutex_);

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
    MutexLockGuard lock(rfidMutex_);

    gpio_num_t rstPin = RFIDConfig::rstPin;

    if (gpio_set_level(rstPin, 0) != ESP_OK) {
        throw std::runtime_error("Failed to set reset pin LOW");
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    if (gpio_set_level(rstPin, 1) != ESP_OK) {
        throw std::runtime_error("Failed to set reset pin HIGH");
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    if (!writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::SoftReset))) {
        throw std::runtime_error("Failed to reset RFID reader");
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    return true;
}
