#ifdef EMBEDDED_BUILD
#pragma once

#include "driver/rmt.h"

class IRTransmitter {
public:
    IRTransmitter(gpio_num_t gpio_num, rmt_channel_t channel);
    void sendCommand(uint32_t address, uint32_t command);

private:
    gpio_num_t gpio_num_;
    rmt_channel_t channel_;
};

#endif // EMBEDDED_BUILD
