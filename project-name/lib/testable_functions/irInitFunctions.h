#pragma once

namespace IRDevices {
    // vectors to hold receivers & transmitters instances
    std::vector<std::unique_ptr<IRReceiver>> irReceivers;
    std::vector<std::unique_ptr<IRTransmitter>> irTransmitters;
}

void initReceivers(std::vector<std::unique_ptr<IRReceiver>> irReceivers);
void initTransmitters(std::vector<std::unique_ptr<IRTransmitter>> irTransmitters);
void initIR();