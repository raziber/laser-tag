#pragma once

void initReceivers(std::vector<std::unique_ptr<IRReceiver>> irReceivers);
void initTransmitters(std::vector<std::unique_ptr<IRTransmitter>> irTransmitters);