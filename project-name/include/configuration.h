#pragma once

#include <array>

// -----------------------------------------------------
// ----------------- IR communications -----------------
// -----------------------------------------------------

// GPIO pins for IR sensors and transmitters
// make sure support by the RMT peripheral
// allowed pins on the ESP32:
/*
GPIO 0  -> 19
GPIO 21 -> 23
GPIO 25 -> 27
GPIO 32 -> 33
INPUT ONLY: GPIO 34 -> 39

*/
// maximum RMT_CHANNEL_MAX channels
namespace IRConfig{
    static constexpr int numReceiverPins = 2;
    static constexpr int numTransmitterPins = 2;
    static constexpr std::array<int, numReceiverPins> sensorPins = {19, 21};
    static constexpr std::array<int, numTransmitterPins> shooterPins = {32, 33};
}