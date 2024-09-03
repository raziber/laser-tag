#pragma once

#include <string>

namespace BT {
    void bluetoothTask(void *parameter);
    void begin();
    void disconnectFromDevice();
    bool connectToDevice(const std::string& address);
}
