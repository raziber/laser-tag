#pragma once

#include "driver/rmt.h"

// RMT settings
namespace rmtSettings{
    namespace receiver{
        constexpr bool RECEIVER_FILTER_ENABLE = true;
        constexpr int IDLE_THRESHOLD = 1200;
        constexpr int FILTER_TICKS_THRESH = 100;
        constexpr int FLAGS = 0;
    }

    namespace trasmitter{
        constexpr bool LOOP_ENABLE = false;
        constexpr bool CARRIER_ENABLE = true;
        constexpr bool OUTPUT_ENABLE = true;
        constexpr rmt_idle_level_t IDLE_LEVEL = RMT_IDLE_LEVEL_LOW;
        constexpr rmt_carrier_level_t CARRIER_LEVEL = RMT_CARRIER_LEVEL_HIGH;
        constexpr int CARRIER_FREQ_HZ = 38000;
        constexpr int CARRIER_DUTY_PERCENTAGE = 33;
        constexpr int TIMEOUT_MS = 1000;
    }

    constexpr int CLK_DIV = 80;
    constexpr int RX_BUFFER_SIZE = 1024;
    constexpr int MEM_BLOCK_NUM = 1;
}
