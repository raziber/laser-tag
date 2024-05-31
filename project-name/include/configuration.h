#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// -----------------------------------------------------
// ----------------- IR communications -----------------
// -----------------------------------------------------

// GPIO pins for IR sensors
// make sure these ouns support the RMT peripheral
#define SENSOR_PINS {19, 21}

// communication protocol, uncomment desired protocol
// #define NEC
#define SAMSUNG
// #define LTTO
// #define CUSTOM_PROTOCOL

#endif // CONFIGURATION_H
