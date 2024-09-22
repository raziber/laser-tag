#pragma once

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
