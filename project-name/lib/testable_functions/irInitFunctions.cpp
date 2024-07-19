#include "irInitFunctions.h"

#include "IRReceiver.h"
#include "IRTransmitter.h"

void initReceivers(std::vector<std::unique_ptr<IRReceiver>> irReceivers) {
    int channel = 0;

    for (int i = 0; i < NUM_SENSORS; ++i) {
        if (channel >= RMT_CHANNEL_MAX) {
            Serial.printf("Exceeded max RMT channels for receivers at channel %d\n", channel);
            return;
        }
        irReceivers.push_back(std::make_unique<IRReceiver>((gpio_num_t)irSettings::SENSOR_PINS[i], (rmt_channel_t)channel++));
        irReceivers.back()->init();
    }
}

void initTransmitters(std::vector<std::unique_ptr<IRTransmitter>> irTransmitters) {
    int channel = 0;
    
    for (int i = 0; i < NUM_SHOOTERS; ++i) {
        if (channel >= RMT_CHANNEL_MAX) {
            Serial.printf("Exceeded max RMT channels for transmitters at channel %d\n", channel);
            return;
        }
        irTransmitters.push_back(std::make_unique<IRTransmitter>((gpio_num_t)irSettings::SHOOTER_PINS[i], (rmt_channel_t)channel++));
        irTransmitters.back()->init();
    }
}

void initIR(){
    initReceivers(IRDevices::irReceivers);
    initTransmitters(IRDevices::irTransmitters);
    Serial.println("All IR receivers and transmitters initialized.");
}