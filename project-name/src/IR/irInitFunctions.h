#pragma once

#include "IRReceiver.h"
#include "IRTransmitter.h"
#include "Encoder.h"
#include "Decoder.h"
#include "IProtocolSettings.h"
#include <vector>
#include <memory>

namespace IRDevices {
    // vectors to hold receivers & transmitters instances
    extern std::vector<std::unique_ptr<IRReceiver>> irReceivers;
    extern std::vector<std::unique_ptr<IRTransmitter>> irTransmitters;
}

void initReceivers(const Decoder* decoder, const IProtocolSettings* settings);
void initTransmitters(const Encoder* encoder, const IProtocolSettings* settings);
void initIR(const Encoder* encoder, const Decoder* decoder, const IProtocolSettings* settings);