#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// IR sensors
#define SENSOR_PINS {19}      // GPIO pins for IR sensors
static const int sensorPins[] = SENSOR_PINS;
#define NUM_SENSORS (sizeof(sensorPins) / sizeof(sensorPins[0]))

// RMT settings
#define RMT_CLK_DIV 80
#define RMT_RX_BUFFER_SIZE 1024

// IR communication protocol
#define LTTO_BASE_ZERO 600   // Base duration for a '0' bit in microseconds
#define LTTO_BASE_ONE 1200   // Base duration for a '1' bit in microseconds
#define LTTO_TOLERANCE 100   // Tolerance in microseconds
#define LTTO_START_BIT 2400  // Duration for the start bit in microseconds


#endif // CONFIGURATION_H
