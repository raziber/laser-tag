#pragma once

#include "IRReceiver.h"
#include "IRTransmitter.h"
#include <vector>
#include <memory>

namespace IRDevices {
    // vectors to hold receivers & transmitters instances
    std::vector<std::unique_ptr<IRReceiver>> irReceivers;
    std::vector<std::unique_ptr<IRTransmitter>> irTransmitters;
}

void initReceivers();
void initTransmitters();
void initIR();