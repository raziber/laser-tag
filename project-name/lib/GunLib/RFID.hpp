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
    void initResetPin();
    void reset();
    void toggleResetPin();
    void performSoftReset();
    void initializeCommunication();

    // === UID Retrieval and Conversion ===
    void retrieveUID(std::vector<uint8_t>& uid);
    std::string convertUIDToString(const std::vector<uint8_t>& uid);

    // === RFID Command Methods ===
    void antennaOn();
    void request(uint8_t requestMode, std::vector<uint8_t>& atqa);
    void selectTag(std::vector<uint8_t>& uid);

    // === Tag Selection Helpers ===
    void performAntiCollision(std::vector<uint8_t>& uidComplete);
    void validateBCC(const std::vector<uint8_t>& uidComplete);
    void constructSelectCommand(const std::vector<uint8_t>& uidComplete, std::vector<uint8_t>& selectCommand);
    void executeSelectCommand(const std::vector<uint8_t>& selectCommand);

    // === Communication with PICC ===
    void communicateWithPICC(Command command, const std::vector<uint8_t>& sendData, std::vector<uint8_t>& backData);

    // === Communication Helper Methods ===
    void prepareForCommunication();
    void writeToFIFO(const std::vector<uint8_t>& data);
    void startCommand(Command command);
    void waitForCommandCompletion(uint8_t waitIRq);
    void checkForErrors();
    void readFromFIFO(std::vector<uint8_t>& data);

    // === CRC Calculation ===
    void calculateCRC(const std::vector<uint8_t>& data, std::vector<uint8_t>& result);

    // === CRC Calculation Helpers ===
    void prepareForCRCCalculation();
    void writeDataForCRC(const std::vector<uint8_t>& data);
    void startCRCCalculation();
    void waitForCRCCompletion();
    void readCRCResult(std::vector<uint8_t>& result);

    // === Low-Level Register Access ===
    uint8_t readRegister(Register reg);
    void writeRegister(Register reg, uint8_t value);
};
