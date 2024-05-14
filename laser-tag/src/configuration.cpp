#include "configuration.h"
#include <Arduino.h>

// Pointers for dynamically allocated arrays
int *ports = nullptr;
int numPorts = 0;

void initConfiguration() {
    static const int defaultPorts[] = {PORTS};

    numPorts = sizeof(defaultPorts) / sizeof(defaultPorts[0]);

    // Allocate the ports and sensorFlags arrays dynamically
    ports = new int[numPorts];

    // Initialize arrays
    for (int i = 0; i < numPorts; i++) {
        ports[i] = defaultPorts[i];
    }
}