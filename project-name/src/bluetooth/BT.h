#pragma once

#include <string>

namespace BT {
    void bluetoothTask(void *parameter);
    void bluetoothInit();
    void disconnectFromDevice();
    bool connectToDevice(const std::string& address);
}
