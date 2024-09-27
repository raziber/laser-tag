#pragma once

#include <Arduino.h>
#include "RFIDReader.hpp"
#include "SimpleIRTransmitter.hpp"
#include "ButtonHandler.hpp"
#include "Player.hpp"
#include "SPIBus.hpp"
#include "SPIDevice.hpp"

class LaserTagGun {
public:
    std::optional<LaserTagGun> make();

    LaserTagGun(gpio_num_t irLedPin, gpio_num_t buttonPin, IRProtocol protocol, std::unique_ptr<RFIDReader> rfidReader);
    ~LaserTagGun();

    esp_err_t start();

private:
    gpio_num_t irLedPin_;
    gpio_num_t buttonPin_;
    IRProtocol protocol_;

    // Task functions
    static void rfidTask(void* arg);
    static void buttonTask(void* arg);

    // Event handlers
    void handleRFIDEvent(const std::string& tagId);
    void handleButtonEvent();

    // Helper methods
    void fire();

    // Components
    std::unique_ptr<RFIDReader> rfidReader_;
    SimpleIRTransmitter irTransmitter_;
    ButtonHandler buttonHandler_;
    Player currentPlayer_;

    // FreeRTOS handles
    TaskHandle_t rfidTaskHandle_;
    TaskHandle_t buttonTaskHandle_;

    // Synchronization primitives
    SemaphoreHandle_t playerMutex_;

    // Internal state
    uint32_t personalCode_;
};
