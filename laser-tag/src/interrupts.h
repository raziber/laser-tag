#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <Arduino.h>
#include "configuration.h"

extern volatile bool interruptEnabled;
extern volatile bool *sensorFlags;

void IRAM_ATTR genericISR(void *arg);
void sensorsInterruptsSetUp();

#endif
