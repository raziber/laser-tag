#pragma once

#include <driver/spi_master.h>
#include <driver/gpio.h>

namespace RFIDConfig {
    gpio_num_t misoPin = defaultMisoPin;
    gpio_num_t mosiPin = defaultMosiPin;
    gpio_num_t clkPin  = defaultClkPin;
    gpio_num_t csPin   = defaultCsPin;
    gpio_num_t rstPin  = defaultRstPin;

    static constexpr int spiClockSpeedHz = 5000000; // 5 MHz
}
