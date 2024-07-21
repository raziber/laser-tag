#include "irInitFunctions.h"

#include "configurationBackend.h"
#include "driver/rmt.h"
#include <Arduino.h>
#include "utils.h"

namespace IRDevices {
    std::vector<std::unique_ptr<IRReceiver>> irReceivers;
    std::vector<std::unique_ptr<IRTransmitter>> irTransmitters;
}

void initReceivers() {
    int channel = 0;

    for (int i = 0; i < irSettings::NUM_SENSORS; ++i) {
        if (channel >= RMT_CHANNEL_MAX) {
            Utils::safeSerialPrintf("Exceeded max RMT channels for receivers at channel %d\n", channel);
            return;
        }
        IRDevices::irReceivers.push_back(std::make_unique<IRReceiver>((gpio_num_t)irSettings::SENSOR_PINS[i], (rmt_channel_t)channel++));
        IRDevices::irReceivers.back()->init();
    }
}

void initTransmitters() {
    int channel = 0;
    
    for (int i = 0; i < irSettings::NUM_SHOOTERS; ++i) {
        if (channel >= RMT_CHANNEL_MAX) {
            Utils::safeSerialPrintf("Exceeded max RMT channels for transmitters at channel %d\n", channel);
            return;
        }
        IRDevices::irTransmitters.push_back(std::make_unique<IRTransmitter>((gpio_num_t)irSettings::SHOOTER_PINS[i], (rmt_channel_t)channel++));
        IRDevices::irTransmitters.back()->init();
    }
}

void initIR(){
    initReceivers();
    initTransmitters();
    Utils::safeSerialPrintln("All IR receivers and transmitters initialized.");
}