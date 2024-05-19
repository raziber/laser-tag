#ifdef EMBEDDED_BUILD
#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#define IR_RESOLUTION_HZ 1000000 // 1MHz resolution

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

#endif // IR_RECEIVER_H
#endif // EMBEDDED_BUILD
