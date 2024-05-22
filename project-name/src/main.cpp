#ifdef EMBEDDED_BUILD

#include "IRReceiver.h"

// Create an instance of the IRNecReceiver class
IRReceiver irReceiver1(GPIO_NUM_19, RMT_CHANNEL_0);
IRReceiver irReceiver2(GPIO_NUM_21, RMT_CHANNEL_1);

void setup() {
    Serial.begin(115200);
    irReceiver1.init(); // Initialize the IR receiver
    irReceiver2.init(); // Initialize the IR receiver
}

void loop() {
    // Main loop does nothing, IR receive task handles everything
    vTaskDelay(pdMS_TO_TICKS(1000)); // Sleep to let FreeRTOS manage tasks
}

#else
int main(){
    
}
#endif
