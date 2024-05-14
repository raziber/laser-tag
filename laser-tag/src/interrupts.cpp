#include "interrupts.h"
#include "configuration.h"
#include <Arduino.h>

volatile bool interruptEnabled = true;
volatile bool *sensorFlags = nullptr;

// ISR that receives a specific pin index as an argument
void IRAM_ATTR genericISR(void *arg) {
    // block all other interrupts
    interruptEnabled = false;

    // tell sensor task to start proccessing a hit
    int index = *((int *)arg);
    sensorFlags[index] = true;
}

// Setup interrupts with arguments
void sensorsInterruptsSetUp() {
    int *indices = new int[numPorts];
    sensorFlags = new volatile bool[numPorts];

    for (int i = 0; i < numPorts; i++) {
        sensorFlags[i] = false;

        indices[i] = i;  // Store the index in a static array
        pinMode(ports[i], INPUT_PULLUP);  // Set pin mode
        attachInterruptArg(digitalPinToInterrupt(ports[i]), genericISR, &indices[i], RISING);  // Pass index
    }

    #ifdef SERIAL_PRINT
        Serial.println("All sensor interrupts configured successfully.");
    #endif

    delete[] indices;  // Cleanup
}
