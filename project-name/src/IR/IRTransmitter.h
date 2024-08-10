#ifdef EMBEDDED_BUILD
#pragma once

#include "driver/rmt.h"
#include "IProtocolSettings.h"
#include "Encoder.h"

class IRTransmitter {
public:
    IRTransmitter(gpio_num_t gpio_num, rmt_channel_t channel, const IProtocolSettings* protocolSettings, const Encoder* encoder);
    ~IRTransmitter();
    void sendCommand(uint32_t address, uint32_t command);

private:
    gpio_num_t gpio_num_;
    rmt_channel_t channel_;
    const IProtocolSettings* settings;
    const Encoder* encoder;
};

#endif // EMBEDDED_BUILD
