#ifdef EMBEDDED_BUILD
#ifndef IR_NEC_RECEIVER_H
#define IR_NEC_RECEIVER_H

#include <Arduino.h>
#include "driver/rmt.h"

#define IR_RESOLUTION_HZ 1000000 // 1MHz resolution
#define NEC_DECODE_MARGIN 200    // Increase decode margin to 300

#define NEC_LEADING_CODE_DURATION_0  9000
#define NEC_LEADING_CODE_DURATION_1  4500
#define NEC_PAYLOAD_ZERO_DURATION_0  560
#define NEC_PAYLOAD_ZERO_DURATION_1  560
#define NEC_PAYLOAD_ONE_DURATION_0   560
#define NEC_PAYLOAD_ONE_DURATION_1   1690
#define NEC_REPEAT_CODE_DURATION_0   9000
#define NEC_REPEAT_CODE_DURATION_1   2250

class IRNecReceiver {
public:
    IRNecReceiver(gpio_num_t gpio_num, rmt_channel_t channel);
    void init();
    static void receiveTask(void* param);

private:
    gpio_num_t gpio_num_;
    rmt_channel_t channel_;
    RingbufHandle_t rb_;

    static bool checkInRange(uint32_t duration, uint32_t spec);
    static bool parseLogic0(rmt_item32_t* item);
    static bool parseLogic1(rmt_item32_t* item);
    static bool parseFrame(rmt_item32_t* items, uint16_t& address, uint16_t& command);
    static bool parseRepeatFrame(rmt_item32_t* items);
    void handleReceivedData();
};

#endif // IR_NEC_RECEIVER_H
#endif // EMBEDDED_BUILD
