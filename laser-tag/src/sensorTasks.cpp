#include "sensorTasks.h"
#include "irCommsFunctions.h"
#include "interrupts.h"
#include "game.h"
#include "configuration.h"

void taskSensorRecord(void* pvParameters){
	int portIndex = *(int*)pvParameters;
	while(true){
		if(sensorFlags[portIndex]){
			recordSensor(portIndex);
			parseResults(portIndex);
            sendResults(portIndex);
			sensorFlags[portIndex] = false;
    		interruptEnabled = true;
		}
	}
}

void setUpSensorTasks(){
    // Array of indices to pass to the ISR
	int* indices = new int[numPorts];

    for (int i = 0; i < numPorts; i++) {
        indices[i] = i;  // Store the index in a static array
		char taskName[21]; // the max length of a task name including null terminator
		snprintf(taskName, sizeof(taskName), "taskSensorRecord%d", i);
        xTaskCreate(taskSensorRecord, taskName, 1024, &indices[i], 1, NULL);
    }

    #ifdef SERIAL_PRINT
        Serial.println("All sensor interrupts configured successfully.");
    #endif
}
