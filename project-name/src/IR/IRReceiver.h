#pragma once
#ifdef EMBEDDED_BUILD

#include "driver/rmt.h"
#include "IProtocolSettings.h"
#include "Decoder.h"

class IRReceiver {
public:
    IRReceiver(gpio_num_t gpio_num, rmt_channel_t channel, const IProtocolSettings* protocolSettings, const Decoder* decoder);
    ~IRReceiver();
    static void receiveTask(void* param);

private:
    gpio_num_t gpio_num_;
    rmt_channel_t channel_;
    RingbufHandle_t rb_;
    const IProtocolSettings* settings;
    const Decoder* decoder;

    void handleReceivedData();
};

#endif // EMBEDDED_BUILD
