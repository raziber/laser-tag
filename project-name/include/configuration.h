#ifndef CONFIGURATION_H
#define CONFIGURATION_H

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
namespace irSettings{
    constexpr std::array<int, 1> SENSOR_PINS = {19};
    constexpr std::array<int, 1> SHOOTER_PINS = {33};
}


// communication protocol, uncomment desired protocol
// #define NEC
#define SAMSUNG
// #define LTTO
// #define CUSTOM_PROTOCOL

#endif // CONFIGURATION_H
