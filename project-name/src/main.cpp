#ifdef EMBEDDED_BUILD

#include <Arduino.h>
#include "configuration.h"
#include "irInitFunctions.h"
#include "BT.h"
#include "ProtocolTypes.h"
#include "GameManager.h"

void setup() {
    static constexpr int BAUD_RATE = 115200;
    Serial.begin(BAUD_RATE);
    BT::begin();
    IRDevices::begin(Protocol::SAMSUNG);

    GameManager::begin();

    // std::string address = "0c:c4:13:17:e4:88";
    // BT::connectToDevice(address);
}

void loop() {
    // Example usage: send a command from the first transmitter
    uint32_t address = 0xF8F8;
    uint32_t command = 0x0BF4;
    // IRDevices::irTransmitters[0]->sendCommand(address, command);

    vTaskDelay(pdMS_TO_TICKS(3000)); // Sleep to let FreeRTOS manage tasks
}

#else
int main(){
    
}
#endif
