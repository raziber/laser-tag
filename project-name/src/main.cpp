#ifdef EMBEDDED_BUILD

#include "IRReceiver.h"
#include "IRTransmitter.h"
#include "irInitFunctions.h"

namespace {
    // vectors to hold receivers & transmitters instances
    std::vector<std::unique_ptr<IRReceiver>> irReceivers;
    std::vector<std::unique_ptr<IRTransmitter>> irTransmitters;
}

void setup() {
    Serial.begin(115200);

    initReceivers(irReceivers);
    initTransmitters(irTransmitters);

    Serial.println("All IR receivers and transmitters initialized.");
}

void loop() {
    // Example usage: send a command from the first transmitter
    uint32_t address = 0xF8F8;
    uint32_t command = 0x0BF4;
    irTransmitters[0]->sendCommand(address, command);

    vTaskDelay(pdMS_TO_TICKS(3000)); // Sleep to let FreeRTOS manage tasks
}

#else
int main(){
    
}
#endif
