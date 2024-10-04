#pragma once

/**
 * @file    IRTransmitter.hpp
 * @brief   Class definition for IRTransmitter.
 *
 * The `IRTransmitter` class provides functionality to transmit infrared (IR) signals using the ESP32's RMT peripheral.
 * It supports asynchronous transmission by utilizing a FreeRTOS task and a queue to handle transmission requests.
 * This class is designed to work with multiple GPIO ports and supports transmitting to all or individual ports.
 *
 * ### Key Features:
 * - Asynchronous transmission using a FreeRTOS task.
 * - Supports multiple GPIO ports for transmission.
 * - Utilizes an `IREncoder` to encode data according to the specified IR protocol.
 * - Thread-safe operations using mutexes.
 *
 * ### Usage Example:
 * ```cpp
 * // Include necessary headers
 * #include "IRTransmitter.hpp"
 * #include "IREncoder.hpp"
 * #include <vector>
 * 
 * // Define GPIO pins for transmission
 * std::vector<int> transmitterGpioPins = {GPIO_NUM_25, GPIO_NUM_26};
 * 
 * // Create an IREncoder instance for the NEC protocol
 * auto encoder = std::make_unique<IREncoder>(IRProtocol::NEC);
 * 
 * // Create an IRTransmitter instance
 * IRTransmitter transmitter(transmitterGpioPins, std::move(encoder), 1, 80);
 * 
 * // Enqueue a transmission to all ports
 * transmitter.transmitToAllPorts(0x10, 0x20);
 * 
 * // Enqueue a transmission to a single port
 * transmitter.transmitToSinglePort(0, 0x10, 0x21);
 * ```
 *
 * ### Notes:
 * - Ensure that the GPIO pins specified are capable of RMT output.
 * - The class manages its own FreeRTOS task and should be properly destructed to free resources.
 * - Transmission requests are non-blocking and are handled asynchronously.
 *
 * @author  Razi Berg
 * @date    23/09/2024
 */

#include "IRProtocols/IRProtocolSettings.hpp"
#include "IREncoder.hpp"
#include <driver/rmt.h>
#include <memory>
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"

struct TransmissionRequest {
    uint32_t address;
    uint32_t command;
    uint32_t portIndex;  // Use UINT32_MAX to indicate all ports
};

class IRTransmitter {
public:
    IRTransmitter(const std::vector<int>& ports, std::unique_ptr<IREncoder> encoder, int memBlockNum, int clkDiv);
    ~IRTransmitter();

    esp_err_t transmitToAllPorts(uint32_t address, uint32_t command);
    esp_err_t transmitToSinglePort(uint32_t portIndex, uint32_t address, uint32_t command);

private:
    static void transmitterTask(void* arg);

    mutable SemaphoreHandle_t transmitMutex_;
    std::unique_ptr<IREncoder> encoder_;
    std::vector<int> gpioPorts_;
    int memBlockNum_;
    int clkDiv_;

    TaskHandle_t taskHandle_;
    QueueHandle_t transmissionQueue_;

    esp_err_t createPacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command);
    esp_err_t transmitToPort(uint32_t portIndex, std::vector<rmt_item32_t>& packet);
    esp_err_t validatePortIndex(uint32_t portIndex);
    void setPortRmtParams(rmt_config_t& rmt_tx_config, int portIndex, int memBlockNum, int clkDiv);

    esp_err_t configurePort(int port, int memBlockNum, int clkDiv);
    esp_err_t uninstallRmtDriver(rmt_channel_t channel);

    // Prevent copying
    IRTransmitter(const IRTransmitter&) = delete;
    IRTransmitter& operator=(const IRTransmitter&) = delete;
};



void submit(function, function_args)
{

    // runs the function with funciton args in another thread. 
}


void foo(void* a)
{
    
}

submit(foo);

struct RunRequest
{
    void (*void) function_pointer;
    void* function_argument;
}

void task_main {
    while(true)
    {
        RunRequest r = receive();
        r.function_pointer(r.function_argument);
    }
}
