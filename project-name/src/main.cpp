#ifdef EMBEDDED_BUILD

#include "IRReceiver.h"
#include "IRTransmitter.h"

// vectors to hold receivers & transmitters instances
std::vector<std::unique_ptr<IRReceiver>> irReceivers;
std::vector<std::unique_ptr<IRTransmitter>> irTransmitters;

void initReceivers(int& channel) {
    for (int i = 0; i < NUM_SENSORS; ++i) {
        if (channel >= RMT_CHANNEL_MAX) {
            Serial.printf("Exceeded max RMT channels for receivers at channel %d\n", channel);
            return;
        }
        irReceivers.push_back(std::make_unique<IRReceiver>((gpio_num_t)irSettings::SENSOR_PINS[i], (rmt_channel_t)channel++));
        irReceivers.back()->init();
    }
}

void initTransmitters(int& channel) {
    for (int i = 0; i < NUM_SHOOTERS; ++i) {
        if (channel >= RMT_CHANNEL_MAX) {
            Serial.printf("Exceeded max RMT channels for transmitters at channel %d\n", channel);
            return;
        }
        irTransmitters.push_back(std::make_unique<IRTransmitter>((gpio_num_t)irSettings::SHOOTER_PINS[i], (rmt_channel_t)channel++));
        irTransmitters.back()->init();
    }
}

void setup() {
    Serial.begin(115200);

    int channel = 0;
    initReceivers(channel);
    initTransmitters(channel);

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
