#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "irCommsFunctions.h"
#include "sensorTasks.h"
#include "interrupts.h"

void setup() {
	Serial.begin(115200);

	initConfiguration();
	sensorsInterruptsSetUp();
	setUpSensorTasks();
}

void loop(){

}
