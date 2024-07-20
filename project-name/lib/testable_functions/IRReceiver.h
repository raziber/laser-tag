#pragma once
#ifdef EMBEDDED_BUILD

#include "driver/rmt.h"

class IRReceiver {
public:
    IRReceiver(gpio_num_t gpio_num, rmt_channel_t channel);
    void init();
    static void receiveTask(void* param);

private:
    gpio_num_t gpio_num_;
    rmt_channel_t channel_;
    RingbufHandle_t rb_;

    void handleReceivedData();
};

#endif // EMBEDDED_BUILD
