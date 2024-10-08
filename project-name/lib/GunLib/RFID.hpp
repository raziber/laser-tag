// RFID.hpp
#pragma once

#include "SPIDevice.hpp"
#include "MFRC522Commands.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "MutexLockGuard.hpp"
#include "Task.hpp"
#include "Queue.hpp"
#include <vector>
#include <string>
#include <optional>
#include <memory>
#include "MFRC522Registers.hpp"

class RFID {
public:
    RFID(SPIBus& spiBus, int csPin);
    void startMonitoring(Queue<std::string>& rfidQueue);

private:
    SPIDevice spiDevice_;
    SemaphoreHandle_t rfidMutex_;
    std::shared_ptr<Task> readingTask_;

    void updateRead(Queue<std::string>& rfidQueue);
    std::string readPlayerID();

    // Helper methods
    bool antennaOn();
    bool request(uint8_t requestMode, std::vector<uint8_t>& atqa);
    bool selectTag(std::vector<uint8_t>& uid);
    bool communicateWithPICC(Command command, const std::vector<uint8_t>& sendData, std::vector<uint8_t>& backData);
    bool calculateCRC(const std::vector<uint8_t>& data, std::vector<uint8_t>& result);

    // Low-level register access
    std::optional<uint8_t> readRegister(Register reg);
    bool writeRegister(Register reg, uint8_t value);
    bool initResetPin();
    bool reset();
};
