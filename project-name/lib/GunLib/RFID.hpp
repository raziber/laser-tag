// RFID.hpp
#pragma once

// Standard library headers
#include <vector>
#include <string>
#include <optional>
#include <memory>

// FreeRTOS headers
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Project headers
#include "SPIDevice.hpp"
#include "SPIBus.hpp"
#include "MFRC522Commands.hpp"
#include "MFRC522Registers.hpp"
#include "MutexLockGuard.hpp"
#include "Task.hpp"
#include "Queue.hpp"

class RFID {
public:
    // Constructor
    RFID(SPIBus& spiBus, int csPin);

    // Start the RFID monitoring task
    void startMonitoring(std::shared_ptr<Queue<std::string>> rfidQueue);

private:
    // === Member Variables ===
    SPIDevice spiDevice_;
    SemaphoreHandle_t rfidMutex_;
    std::shared_ptr<Task> readingTask_;
    std::shared_ptr<Queue<std::string>> rfidQueue_;

    // === High-Level RFID Operations ===
    void updateRead();
    void processRFIDReading();
    void delayBetweenReadings();
    std::string readPlayerID();

    // === RFID Initialization and Reset ===
    bool initResetPin();
    bool reset();
    bool toggleResetPin();
    bool performSoftReset();
    bool initializeCommunication();

    // === UID Retrieval and Conversion ===
    bool retrieveUID(std::vector<uint8_t>& uid);
    std::string convertUIDToString(const std::vector<uint8_t>& uid);

    // === RFID Command Methods ===
    bool antennaOn();
    bool request(uint8_t requestMode, std::vector<uint8_t>& atqa);
    bool selectTag(std::vector<uint8_t>& uid);

    // === Tag Selection Helpers ===
    bool performAntiCollision(std::vector<uint8_t>& uidComplete);
    bool validateBCC(const std::vector<uint8_t>& uidComplete);
    bool constructSelectCommand(const std::vector<uint8_t>& uidComplete, std::vector<uint8_t>& selectCommand);
    bool executeSelectCommand(const std::vector<uint8_t>& selectCommand);

    // === Communication with PICC ===
    bool communicateWithPICC(Command command, const std::vector<uint8_t>& sendData, std::vector<uint8_t>& backData);

    // === Communication Helper Methods ===
    bool prepareForCommunication();
    bool writeToFIFO(const std::vector<uint8_t>& data);
    bool startCommand(Command command);
    bool waitForCommandCompletion();
    bool checkForErrors();
    bool readFromFIFO(std::vector<uint8_t>& data);

    // === CRC Calculation ===
    bool calculateCRC(const std::vector<uint8_t>& data, std::vector<uint8_t>& result);

    // === CRC Calculation Helpers ===
    bool prepareForCRCCalculation();
    bool writeDataForCRC(const std::vector<uint8_t>& data);
    bool startCRCCalculation();
    bool waitForCRCCompletion();
    bool readCRCResult(std::vector<uint8_t>& result);

    // === Low-Level Register Access ===
    std::optional<uint8_t> readRegister(Register reg);
    bool writeRegister(Register reg, uint8_t value);
};
