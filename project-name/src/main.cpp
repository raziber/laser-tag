#ifdef EMBEDDED_BUILD

#include <Arduino.h>
#include "configuration.h"
#include "irInitFunctions.h"
#include "BT.h"
#include "utils.h"
#include "protocolManager.h"
#include "ProtocolTypes.h"
#include "Decoder.h"
#include "Encoder.h"

void setup() {
    constexpr int BAUD_RATE = 115200;
    Serial.begin(BAUD_RATE);
    BT::bluetoothInit();

    ProtocolManager protocolManager;
    protocolManager.selectProtocol(Protocol::SAMSUNG);                  // Select a protocol at runtime
    const IProtocolSettings* settings = protocolManager.getSettings();  // Retrieve settings

    auto decoder = std::make_unique<Decoder>(settings);
    auto encoder = std::make_unique<Encoder>(settings);

    initIR(encoder.get(), decoder.get(), settings);

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
