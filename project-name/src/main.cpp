#ifdef EMBEDDED_BUILD

#include <Arduino.h>
#include "configuration.h"
#include "irInitFunctions.h"
#include "BT.h"

void setup() {
    Serial.begin(serialSettings::BAUD_RATE);
    bluetoothInit();
    initIR();
}

void loop() {
    // Example usage: send a command from the first transmitter
    uint32_t address = 0xF8F8;
    uint32_t command = 0x0BF4;
    IRDevices::irTransmitters[0]->sendCommand(address, command);

    vTaskDelay(pdMS_TO_TICKS(3000)); // Sleep to let FreeRTOS manage tasks
}

#else
int main(){
    
}
#endif
