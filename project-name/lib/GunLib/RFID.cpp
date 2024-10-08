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

    if (!reset()) {
        throw std::runtime_error("Failed to reset RFID module");
    }
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
    while (!readingTask_->shouldStop()) {
        processRFIDReading();
        delayBetweenReadings();
    }
}

void RFID::processRFIDReading() {
    std::string playerID = readPlayerID();

    if (!playerID.empty()) {
        if (!rfidQueue_->send(playerID)) {
            ESP_LOGE("RFID", "Failed to send player ID to queue: Queue might be full");
        }
    }
}

void RFID::delayBetweenReadings() {
    vTaskDelay(pdMS_TO_TICKS(500));  // Adjust delay as needed
}

bool RFID::antennaOn() {
    MutexLockGuard lock(rfidMutex_);

    auto regValue = readRegister(Register::TxControlRegister);
    if (!regValue.has_value()) {
        ESP_LOGE("RFID", "Failed to read TxControlRegister");
        return false;
    }
    uint8_t txControlReg = regValue.value();
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

    if (!prepareForCommunication()) {
        return false;
    }

    if (!writeToFIFO(sendData)) {
        return false;
    }

    if (!startCommand(command)) {
        return false;
    }

    if (!waitForCommandCompletion()) {
        return false;
    }

    if (!checkForErrors()) {
        return false;
    }

    if (!readFromFIFO(backData)) {
        return false;
    }

    return true;
}

bool RFID::prepareForCommunication() {
    if (!writeRegister(Register::CommIrqReg, 0x7F)) {
        ESP_LOGE("RFID", "Failed to clear interrupt flags");
        return false;
    }

    if (!writeRegister(Register::FIFOLevelRegister, 0x80)) {
        ESP_LOGE("RFID", "Failed to flush FIFO buffer");
        return false;
    }

    return true;
}

bool RFID::writeToFIFO(const std::vector<uint8_t>& data) {
    for (uint8_t byte : data) {
        if (!writeRegister(Register::FIFODataRegister, byte)) {
            ESP_LOGE("RFID", "Failed to write to FIFO");
            return false;
        }
    }
    return true;
}

bool RFID::startCommand(Command command) {
    if (!writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::Idle))) {
        ESP_LOGE("RFID", "Failed to set CommandRegister to Idle");
        return false;
    }

    if (!writeRegister(Register::BitFramingRegister, 0x00)) {
        ESP_LOGE("RFID", "Failed to set BitFramingRegister");
        return false;
    }

    if (!writeRegister(Register::CommandRegister, static_cast<uint8_t>(command))) {
        ESP_LOGE("RFID", "Failed to start command");
        return false;
    }

    return true;
}

bool RFID::waitForCommandCompletion() {
    const uint8_t waitIRq = 0x30;  // RxIRq and IdleIRq
    bool commandCompleted = false;

    for (int i = 0; i < 200; ++i) {
        uint8_t irqReg = readRegister(Register::CommIrqReg).value_or(0);
        if (irqReg & waitIRq) {
            commandCompleted = true;
            break;  // Command completed
        }
        vTaskDelay(pdMS_TO_TICKS(1));  // Delay 1ms
    }

    if (!commandCompleted) {
        ESP_LOGE("RFID", "Timeout waiting for command to complete");
        return false;
    }

    return true;
}

bool RFID::checkForErrors() {
    uint8_t errorReg = readRegister(Register::ErrorReg).value_or(0);
    if (errorReg & 0x13) {  // Buffer overflow, parity error, protocol error
        ESP_LOGE("RFID", "Communication error: 0x%02X", errorReg);
        return false;
    }
    return true;
}

bool RFID::readFromFIFO(std::vector<uint8_t>& data) {
    uint8_t fifoLevel = readRegister(Register::FIFOLevelRegister).value_or(0);
    if (fifoLevel == 0) {
        ESP_LOGE("RFID", "No data in FIFO");
        return false;
    }

    for (uint8_t i = 0; i < fifoLevel; ++i) {
        uint8_t byte = readRegister(Register::FIFODataRegister).value_or(0);
        data.push_back(byte);
    }
    return true;
}

bool RFID::selectTag(std::vector<uint8_t>& uid) {
    MutexLockGuard lock(rfidMutex_);

    std::vector<uint8_t> uidComplete;
    if (!performAntiCollision(uidComplete)) {
        return false;
    }

    if (!validateBCC(uidComplete)) {
        return false;
    }

    // UID bytes are uidComplete[0..3]
    uid.assign(uidComplete.begin(), uidComplete.begin() + 4);

    std::vector<uint8_t> selectCommand;
    if (!constructSelectCommand(uidComplete, selectCommand)) {
        return false;
    }

    if (!executeSelectCommand(selectCommand)) {
        return false;
    }

    // UID successfully read and tag selected
    return true;
}

bool RFID::performAntiCollision(std::vector<uint8_t>& uidComplete) {
    std::vector<uint8_t> buffer = { static_cast<uint8_t>(PICCCommand::AntiCollisionCL1), 0x20 };

    if (!communicateWithPICC(Command::Transceive, buffer, uidComplete)) {
        ESP_LOGE("RFID", "Failed during anti-collision");
        return false;
    }

    if (uidComplete.size() != 5) {
        ESP_LOGE("RFID", "Invalid UID size during anti-collision: %d", uidComplete.size());
        return false;
    }

    return true;
}

bool RFID::validateBCC(const std::vector<uint8_t>& uidComplete) {
    uint8_t bcc = uidComplete[4];
    uint8_t calculatedBCC = uidComplete[0] ^ uidComplete[1] ^ uidComplete[2] ^ uidComplete[3];
    if (bcc != calculatedBCC) {
        ESP_LOGE("RFID", "BCC mismatch");
        return false;
    }
    return true;
}

bool RFID::constructSelectCommand(const std::vector<uint8_t>& uidComplete, std::vector<uint8_t>& selectCommand) {
    selectCommand.clear();
    selectCommand.push_back(static_cast<uint8_t>(PICCCommand::SelectCascadeLevel1));
    selectCommand.push_back(0x70);  // Select command
    selectCommand.insert(selectCommand.end(), uidComplete.begin(), uidComplete.end());  // UID with BCC

    // Calculate CRC_A
    std::vector<uint8_t> crc;
    if (!calculateCRC(selectCommand, crc)) {
        ESP_LOGE("RFID", "Failed to calculate CRC");
        return false;
    }
    selectCommand.insert(selectCommand.end(), crc.begin(), crc.end());

    return true;
}

bool RFID::executeSelectCommand(const std::vector<uint8_t>& selectCommand) {
    std::vector<uint8_t> sak;  // Select Acknowledge
    if (!communicateWithPICC(Command::Transceive, selectCommand, sak)) {
        ESP_LOGE("RFID", "Failed during tag selection");
        return false;
    }

    if (sak.empty() || (sak[0] & 0x04)) {
        ESP_LOGE("RFID", "No SAK received or UID not complete");
        return false;
    }

    return true;
}

bool RFID::calculateCRC(const std::vector<uint8_t>& data, std::vector<uint8_t>& result) {
    MutexLockGuard lock(rfidMutex_);

    if (!prepareForCRCCalculation()) {
        return false;
    }

    if (!writeDataForCRC(data)) {
        return false;
    }

    if (!startCRCCalculation()) {
        return false;
    }

    if (!waitForCRCCompletion()) {
        return false;
    }

    if (!readCRCResult(result)) {
        return false;
    }

    return true;
}

bool RFID::prepareForCRCCalculation() {
    if (!writeRegister(Register::DivIrqReg, 0x04)) {
        ESP_LOGE("RFID", "Failed to clear DivIrqReg");
        return false;
    }
    if (!writeRegister(Register::FIFOLevelRegister, 0x80)) {
        ESP_LOGE("RFID", "Failed to flush FIFO buffer");
        return false;
    }
    return true;
}

bool RFID::writeDataForCRC(const std::vector<uint8_t>& data) {
    return writeToFIFO(data);
}

bool RFID::startCRCCalculation() {
    if (!writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::CalculateCRC))) {
        ESP_LOGE("RFID", "Failed to start CRC calculation");
        return false;
    }
    return true;
}

bool RFID::waitForCRCCompletion() {
    bool crcCompleted = false;
    const int maxCycles = 5000;
    for (int i = 0; i < maxCycles; ++i) {
        uint8_t irqReg = readRegister(Register::DivIrqReg).value_or(0);
        if (irqReg & 0x04) {
            crcCompleted = true;
            break;  // CRC calculation completed
        }
        vTaskDelay(pdMS_TO_TICKS(1));  // Optional delay
    }

    if (!crcCompleted) {
        ESP_LOGE("RFID", "Timeout during CRC calculation");
        return false;
    }

    return true;
}

bool RFID::readCRCResult(std::vector<uint8_t>& result) {
    uint8_t crcA = readRegister(Register::CRCResultLowRegister).value_or(0);
    uint8_t crcB = readRegister(Register::CRCResultHighRegister).value_or(0);

    result = { crcA, crcB };
    return true;
}

std::string RFID::readPlayerID() {
    MutexLockGuard lock(rfidMutex_);

    if (!initializeCommunication()) {
        return "";
    }

    std::vector<uint8_t> uid;
    if (!retrieveUID(uid)) {
        return "";
    }

    std::string playerID = convertUIDToString(uid);

    ESP_LOGI("RFID", "Read Player ID: %s", playerID.c_str());
    return playerID;
}

bool RFID::initializeCommunication() {
    if (!antennaOn()) {
        ESP_LOGE("RFID", "Failed to turn on antenna");
        return false;
    }
    return true;
}

bool RFID::retrieveUID(std::vector<uint8_t>& uid) {
    // Send a Request command to search for tags
    std::vector<uint8_t> atqa;
    if (!request(static_cast<uint8_t>(PICCCommand::RequestA), atqa)) {
        // No tag found
        return false;
    }

    // Select the tag to retrieve its UID
    if (!selectTag(uid)) {
        ESP_LOGE("RFID", "Failed to select tag");
        return false;
    }

    return true;
}

std::string RFID::convertUIDToString(const std::vector<uint8_t>& uid) {
    std::stringstream uidStream;
    uidStream << std::hex << std::setfill('0');
    for (uint8_t byte : uid) {
        uidStream << std::setw(2) << static_cast<int>(byte);
    }
    return uidStream.str();
}

std::optional<uint8_t> RFID::readRegister(Register reg) {
    MutexLockGuard lock(rfidMutex_);

    uint8_t address = ((static_cast<uint8_t>(reg) << 1) & 0x7E) | 0x80;
    uint8_t data[2] = { address, 0 };

    spi_transaction_t transaction = {};
    transaction.length = 8 * sizeof(data);
    transaction.tx_buffer = data;
    transaction.rx_buffer = data;

    // Add the flags here
    transaction.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;

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

    // Add the flags here
    transaction.flags = SPI_TRANS_USE_TXDATA;

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

    // Set reset pin to HIGH (inactive)
    if (gpio_set_level(RFIDConfig::rstPin, 1) != ESP_OK) {
        return false;
    }

    return true;
}

bool RFID::reset() {
    MutexLockGuard lock(rfidMutex_);

    if (!toggleResetPin()) {
        return false;
    }

    if (!performSoftReset()) {
        return false;
    }

    return true;
}

bool RFID::toggleResetPin() {
    gpio_num_t rstPin = RFIDConfig::rstPin;

    if (gpio_set_level(rstPin, 0) != ESP_OK) {
        ESP_LOGE("RFID", "Failed to set reset pin LOW");
        return false;
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    if (gpio_set_level(rstPin, 1) != ESP_OK) {
        ESP_LOGE("RFID", "Failed to set reset pin HIGH");
        return false;
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    return true;
}

bool RFID::performSoftReset() {
    if (!writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::SoftReset))) {
        ESP_LOGE("RFID", "Failed to reset RFID reader");
        return false;
    }
    vTaskDelay(pdMS_TO_TICKS(50));
    return true;
}
