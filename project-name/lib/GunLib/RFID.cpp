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
    
    initResetPin();
    reset();
}

void RFID::startMonitoring(std::shared_ptr<Queue<std::string>> rfidQueue) {
    rfidQueue_ = rfidQueue;

    // Create the task
    try {
        readingTask_ = std::make_shared<Task>(
            "RfidReadingTask",
            4096,   // Stack size in bytes
            1,      // Priority
            [this]() { updateRead(); }  // No need to capture rfidQueue
        );
    } catch (const std::runtime_error& e) {
        ESP_LOGE("RFID", "Failed to create RFID reader reading task: %s", e.what());
    }
}

void RFID::updateRead() {
    if (!readingTask_) {
        throw std::runtime_error("Reading task is not initialized");
    }

    while (!readingTask_->shouldStop()) {
        processRFIDReading();
        delayBetweenReadings();
    }
}

void RFID::processRFIDReading() {
    try {
        std::string playerID = readPlayerID();
        if (!playerID.empty()) {
            if (!rfidQueue_->send(playerID)) {
                throw std::runtime_error("RFID - Failed to send player ID to queue: Queue might be full");
            }
        }
    } catch (const std::exception& e) {
        ESP_LOGE("RFID", "Error during RFID reading: %s", e.what());
    }
}

void RFID::delayBetweenReadings() {
    vTaskDelay(pdMS_TO_TICKS(500));  // Adjust delay as needed
}

void RFID::antennaOn() {
    uint8_t regValue = readRegister(Register::TxControlRegister);
    if (!(regValue & 0x03)) {
        writeRegister(Register::TxControlRegister, regValue | 0x03);
    }
}

void RFID::request(uint8_t requestMode, std::vector<uint8_t>& atqa) {
    MutexLockGuard lock(rfidMutex_);

    // Set BitFramingRegister to start sending at the first bit
    writeRegister(Register::BitFramingRegister, 0x07);

    std::vector<uint8_t> sendData = { requestMode };

    communicateWithPICC(Command::Transceive, sendData, atqa);

    if (atqa.size() != 2) {
        ESP_LOGE("RFID", "Invalid ATQA size: %d", atqa.size());
        throw std::runtime_error("Invalid ATQA size");
    }
}

void RFID::communicateWithPICC(Command command, const std::vector<uint8_t>& sendData, std::vector<uint8_t>& backData) {
    MutexLockGuard lock(rfidMutex_);

    prepareForCommunication();
    writeToFIFO(sendData);
    startCommand(command);

    uint8_t waitIRq = 0x30;  // Default waitIRq, adjust as needed

    if (command == Command::Transceive) {
        waitIRq = 0x30;  // RxIRq and IdleIRq
    } else if (command == Command::Idle) {
        waitIRq = 0x10;  // IdleIRq
    }
    // Add other commands as needed
    waitForCommandCompletion(waitIRq);
    checkForErrors();
    readFromFIFO(backData);
}

void RFID::prepareForCommunication() {
    writeRegister(Register::CommIrqReg, 0x7F);
    writeRegister(Register::FIFOLevelRegister, 0x80);
}

void RFID::writeToFIFO(const std::vector<uint8_t>& data) {
    for (uint8_t byte : data) {
        writeRegister(Register::FIFODataRegister, byte);
    }
}

void RFID::startCommand(Command command) {
    writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::Idle));
    writeRegister(Register::BitFramingRegister, 0x00);
    writeRegister(Register::CommandRegister, static_cast<uint8_t>(command));
}

void RFID::waitForCommandCompletion(uint8_t waitIRq) {
    bool commandCompleted = false;

    for (int i = 0; i < 200; ++i) {
        uint8_t irqRegValue = readRegister(Register::CommIrqReg);

        if (irqRegValue & waitIRq) {
            commandCompleted = true;
            break;  // Command completed
        }
        vTaskDelay(pdMS_TO_TICKS(1));  // Delay 1ms
    }

    if (!commandCompleted) {
        throw std::runtime_error("Timeout waiting for command to complete");
    }
}

void RFID::checkForErrors() {
    uint8_t regValue = readRegister(Register::ErrorReg);

    if (regValue & 0x1B) {  // Check for BufferOvfl, ParityErr, CRCErr, CollErr
        ESP_LOGE("RFID", "Communication error: 0x%02X", regValue);
        throw std::runtime_error("Communication error");
    }
}

void RFID::readFromFIFO(std::vector<uint8_t>& data) {
    uint8_t fifoLevel = readRegister(Register::FIFOLevelRegister);

    for (uint8_t i = 0; i < fifoLevel; ++i) {
        uint8_t byte = readRegister(Register::FIFODataRegister);
        data.push_back(byte);
    }
}

void RFID::selectTag(std::vector<uint8_t>& uid) {
    MutexLockGuard lock(rfidMutex_);

    std::vector<uint8_t> uidComplete;
    performAntiCollision(uidComplete);
    validateBCC(uidComplete);

    // UID bytes are uidComplete[0..3]
    uid.assign(uidComplete.begin(), uidComplete.begin() + 4);

    std::vector<uint8_t> selectCommand;
    constructSelectCommand(uidComplete, selectCommand);
    executeSelectCommand(selectCommand);
}

void RFID::performAntiCollision(std::vector<uint8_t>& uidComplete) {
    std::vector<uint8_t> buffer = { static_cast<uint8_t>(PICCCommand::AntiCollisionCL1), 0x20 };

    communicateWithPICC(Command::Transceive, buffer, uidComplete);

    if (uidComplete.size() != 5) {
        ESP_LOGE("RFID", "Invalid UID size during anti-collision: %d", uidComplete.size());
        throw std::runtime_error("Invalid UID size during anti-collision");
    }
}

void RFID::validateBCC(const std::vector<uint8_t>& uidComplete) {
    uint8_t bcc = uidComplete[4];
    uint8_t calculatedBCC = uidComplete[0] ^ uidComplete[1] ^ uidComplete[2] ^ uidComplete[3];
    if (bcc != calculatedBCC) {
        throw std::runtime_error("BCC mismatch");
    }
}

void RFID::constructSelectCommand(const std::vector<uint8_t>& uidComplete, std::vector<uint8_t>& selectCommand) {
    selectCommand.clear();
    selectCommand.push_back(static_cast<uint8_t>(PICCCommand::SelectCascadeLevel1));
    selectCommand.push_back(0x70);  // Select command
    selectCommand.insert(selectCommand.end(), uidComplete.begin(), uidComplete.end());  // UID with BCC

    // Calculate CRC_A
    std::vector<uint8_t> crc;
    calculateCRC(selectCommand, crc);
    selectCommand.insert(selectCommand.end(), crc.begin(), crc.end());
}

void RFID::executeSelectCommand(const std::vector<uint8_t>& selectCommand) {
    std::vector<uint8_t> sak;  // Select Acknowledge
    communicateWithPICC(Command::Transceive, selectCommand, sak);

    // Check that SAK has exactly one byte
    if (sak.size() != 1) {
        ESP_LOGE("RFID", "Invalid SAK size: %d", sak.size());
        throw std::runtime_error("Invalid SAK size");
    }

    // Check if UID is complete
    if (sak[0] & 0x04) {
        throw std::runtime_error("UID not complete");
    }
}

void RFID::calculateCRC(const std::vector<uint8_t>& data, std::vector<uint8_t>& result) {
    MutexLockGuard lock(rfidMutex_);

    prepareForCRCCalculation();
    writeDataForCRC(data);
    startCRCCalculation();
    waitForCRCCompletion();
    readCRCResult(result);
}

void RFID::prepareForCRCCalculation() {
    writeRegister(Register::DivIrqReg, 0x04);
    writeRegister(Register::FIFOLevelRegister, 0x80);
}

void RFID::writeDataForCRC(const std::vector<uint8_t>& data) {
    writeToFIFO(data);
}

void RFID::startCRCCalculation() {
    writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::CalculateCRC));
}

void RFID::waitForCRCCompletion() {
    bool crcCompleted = false;
    const int maxCycles = 5000;
    for (int i = 0; i < maxCycles; ++i) {
        uint8_t irqReg = readRegister(Register::DivIrqReg);
        if (irqReg & 0x04) {
            crcCompleted = true;
            break;  // CRC calculation completed
        }
        vTaskDelay(pdMS_TO_TICKS(1));  // Optional delay
    }

    if (!crcCompleted) {
        throw std::runtime_error("Timeout during CRC calculation");
    }
}

void RFID::readCRCResult(std::vector<uint8_t>& result) {
    uint8_t crcA = readRegister(Register::CRCResultLowRegister);
    uint8_t crcB = readRegister(Register::CRCResultHighRegister);

    result = { crcA, crcB };
}

std::string RFID::readPlayerID() {
    MutexLockGuard lock(rfidMutex_);

    initializeCommunication();

    std::vector<uint8_t> uid;
    retrieveUID(uid);

    std::string playerID = convertUIDToString(uid);

    ESP_LOGI("RFID", "Read Player ID: %s", playerID.c_str());
    return playerID;
}

void RFID::initializeCommunication() {
    antennaOn();
}

void RFID::retrieveUID(std::vector<uint8_t>& uid) {
    // Send a Request command to search for tags
    std::vector<uint8_t> atqa;
    request(static_cast<uint8_t>(PICCCommand::RequestA), atqa);

    // Select the tag to retrieve its UID
    selectTag(uid);
}

std::string RFID::convertUIDToString(const std::vector<uint8_t>& uid) {
    std::stringstream uidStream;
    uidStream << std::hex << std::setfill('0');
    for (uint8_t byte : uid) {
        uidStream << std::setw(2) << static_cast<int>(byte);
    }
    return uidStream.str();
}

uint8_t RFID::readRegister(Register reg) {
    uint8_t address = ((static_cast<uint8_t>(reg) << 1) & 0x7E) | 0x80;
    uint8_t data[2] = { address, 0 };

    spi_transaction_t transaction = {};
    transaction.length = 8 * sizeof(data);
    transaction.tx_buffer = data;
    transaction.rx_buffer = data;

    transaction.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;

    if (!spiDevice_.transmit(&transaction)) {
        throw std::runtime_error("Failed to read register");
    }

    return data[1];
}

void RFID::writeRegister(Register reg, uint8_t value) {
    uint8_t address = (static_cast<uint8_t>(reg) << 1) & 0x7E;
    uint8_t data[2] = { address, value };

    spi_transaction_t transaction = {};
    transaction.length = 8 * sizeof(data);
    transaction.tx_buffer = data;

    // Add the flags here
    transaction.flags = SPI_TRANS_USE_TXDATA;

    spiDevice_.transmit(&transaction);
}

void RFID::initResetPin() {
    if (gpio_set_direction(RFIDConfig::rstPin, GPIO_MODE_OUTPUT) != ESP_OK) {
        throw std::runtime_error("Failed to set gpio to output");
    }

    // Set reset pin to HIGH (inactive)
    if (gpio_set_level(RFIDConfig::rstPin, 1) != ESP_OK) {
        throw std::runtime_error("Failed to set reset pin to HIGH");
    }
}

void RFID::reset() {
    MutexLockGuard lock(rfidMutex_);

    toggleResetPin();
    performSoftReset();
}

void RFID::toggleResetPin() {
    gpio_num_t rstPin = RFIDConfig::rstPin;

    if (gpio_set_level(rstPin, 0) != ESP_OK) {
        throw std::runtime_error("Failed to set reset pin to LOW");
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    if (gpio_set_level(rstPin, 1) != ESP_OK) {
        throw std::runtime_error("Failed to set reset pin to HIGH");
    }
    vTaskDelay(pdMS_TO_TICKS(50));
}

void RFID::performSoftReset() {
    writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::SoftReset));
    vTaskDelay(pdMS_TO_TICKS(50));
}
