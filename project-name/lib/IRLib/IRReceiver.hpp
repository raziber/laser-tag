#pragma once

/**
 * @file    IRReceiver.hpp
 * @brief   Class definition for IRReceiver.
 *
 * The `IRReceiver` class provides functionality to receive and decode infrared (IR) signals using the ESP32's RMT peripheral.
 * It operates asynchronously by running a FreeRTOS task that continuously listens for incoming IR signals,
 * decodes them using the specified protocol, and posts the decoded data to a FreeRTOS queue.
 *
 * ### Key Features:
 * - Asynchronous reception and decoding using a FreeRTOS task.
 * - Supports multiple protocols through the `IRProtocol` enumeration.
 * - Thread-safe operations using mutexes.
 * - Provides a mechanism to receive decoded data via a FreeRTOS queue.
 *
 * ### Usage Example:
 * ```cpp
 * // Include necessary headers
 * #include "IRReceiver.hpp"
 * 
 * // Define GPIO pin for reception
 * gpio_num_t receiverGpioPin = GPIO_NUM_21;
 * 
 * // Create a queue to receive decoded data
 * QueueHandle_t dataQueue = xQueueCreate(10, sizeof(uint64_t));
 * 
 * // Create an IRReceiver instance
 * IRReceiver receiver(IRProtocol::NEC, receiverGpioPin, 1, 80);
 * receiver.setDataQueue(dataQueue);
 * receiver.start();
 * 
 * // In a FreeRTOS task or main loop
 * uint64_t data;
 * if (xQueueReceive(dataQueue, &data, portMAX_DELAY) == pdTRUE) {
 *     uint32_t address = static_cast<uint32_t>(data >> 32);
 *     uint32_t command = static_cast<uint32_t>(data & 0xFFFFFFFF);
 *     // Process the received address and command
 * }
 * ```
 *
 * ### Notes:
 * - Ensure that the GPIO pin specified is capable of RMT input.
 * - The class manages its own FreeRTOS task and should be properly destructed to free resources.
 * - The data queue must be set using `setDataQueue` before starting the receiver.
 *
 * ### Supported Protocols:
 * - NEC (add others as implemented)
 *
 * ### Important Methods:
 * - `start()`: Starts the receiver task.
 * - `stop()`: Stops the receiver task.
 * - `setDataQueue(QueueHandle_t queue)`: Sets the queue where decoded data will be posted.
 *
 * ### Error Handling:
 * - Methods return `esp_err_t` to indicate success or failure.
 * - Errors are logged using ESP logging facilities for debugging.
 *
 * @see IRDecoder
 * @see IRProtocol
 * @see IRProtocolSettings
 * @see IRDecoder.hpp
 *
 * @author  Razi Berg
 * @date    23/09/2024
 */

#include "IRProtocols/IRProtocolFactory.hpp"
#include "IRProtocols/IRProtocolSettings.hpp"
#include <driver/rmt.h>
#include <Arduino.h>
#include <vector>
#include <memory>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "IRDecoder.hpp"

class IRReceiver {
public:
    IRReceiver(IRProtocol protocol, gpio_num_t gpioNum, int memBlockNum, int clkDiv);
    ~IRReceiver();

    esp_err_t start();
    esp_err_t stop();

    // Set the queue where decoded data will be posted
    void setDataQueue(QueueHandle_t queue);

private:
    static void receiverTask(void* arg);

    void receiveAndProcessData(RingbufHandle_t rb);
    void processReceivedItems(const std::vector<rmt_item32_t>& rawData);
    void handleDecodedData(uint32_t address, uint32_t command);

    mutable SemaphoreHandle_t receiverMutex_;
    IRProtocol protocol_;
    gpio_num_t gpioNum_;
    int memBlockNum_;
    int clkDiv_;
    rmt_channel_t channel_;
    TaskHandle_t taskHandle_;
    QueueHandle_t dataQueue_;

    std::unique_ptr<IRProtocolSettings> protocolSettings_;

    esp_err_t configureRMT();
    esp_err_t uninstallRMT();

    // Prevent copying
    IRReceiver(const IRReceiver&) = delete;
    IRReceiver& operator=(const IRReceiver&) = delete;
};
