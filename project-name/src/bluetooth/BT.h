#pragma once

#include <string>
#include "InputType.h"
#include <Arduino.h>
#include "GameMode.h"

namespace BT {
    void bluetoothTask(void *parameter);
    void begin();
    void disconnectFromDevice();
    bool connectToDevice(const std::string& address);
    InputType ProcessReceivedData(char receivedData);
    InputType ReceiveInput();
    GameMode GetSelectedGameMode();  // Declare the GetSelectedGameMode function

    extern QueueHandle_t btQueue;  // Declare the queue handle
}
