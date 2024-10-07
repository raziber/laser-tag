// RFID.hpp
#pragma once

#include <optional>
#include <memory>
#include <Arduino.h>
#include "SPIDevice.hpp"
#include "Task.hpp"
#include "MFRC522Registers.hpp"
#include "Queue.hpp"

class RFID : public std::enable_shared_from_this<RFID> {
public:
    RFID(SPIBus& spiBus, int csPin);
    ~RFID() = default;

    std::optional<uint8_t> readRegister(Register reg);
    bool reset();
    bool writeRegister(Register reg, uint8_t value);

    void startMonitoring(Queue<std::string>& rfidQueue);

private:
    std::shared_ptr<Task> readingTask_;
    SPIDevice spiDevice_;

    static bool initResetPin();

    // The method to be run in the task
    void updateRead(Queue<std::string>& rfidQueue);
};
