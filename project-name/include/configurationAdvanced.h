#ifndef CONFIGURATION_ADVANCED_H
#define CONFIGURATION_ADVANCED_H

// RMT settings
namespace rmtSettings{
    constexpr int CLK_DIV = 80;
    constexpr int RX_BUFFER_SIZE = 1024;
    constexpr int MEM_BLOCK_NUM = 1;
    constexpr int FLAGS = 0;
    constexpr bool RECEIVER_FILTER_ENABLE = true;
    constexpr int FILTER_TICKS_THRESH = 100;
    constexpr int IDLE_THRESHOLD = 1200;
}

#endif // CONFIGURATION_ADVANCED_H
