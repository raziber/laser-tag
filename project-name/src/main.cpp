#ifdef EMBEDDED_BUILD

#include "IRReceiver.h"

// Create an array to hold IRReceiver instances
IRReceiver* irReceivers[NUM_SENSORS];

void setup() {
    Serial.begin(115200);

    // Initialize each IR receiver
    for (int i = 0; i < NUM_SENSORS; i++) {
        irReceivers[i] = new IRReceiver((gpio_num_t)sensorPins[i], (rmt_channel_t)i);
        irReceivers[i]->init();
    }
}

void loop() {
    // Main loop does nothing, IR receive task handles everything
    vTaskDelay(pdMS_TO_TICKS(1000)); // Sleep to let FreeRTOS manage tasks
}

#else
int main(){
    
}
#endif
