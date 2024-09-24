#pragma once

#include <driver/spi_master.h>
#include <driver/gpio.h>

namespace SPIConfig {
    constexpr spi_host_device_t spiHost = SPI2_HOST;
    constexpr gpio_num_t mosiPin = GPIO_NUM_23;
    constexpr gpio_num_t misoPin = GPIO_NUM_19;
    constexpr gpio_num_t sckPin = GPIO_NUM_18;
    constexpr gpio_num_t ssPin = GPIO_NUM_5;      // Define your SS (CS) pin
    constexpr gpio_num_t rstPin = GPIO_NUM_22;    // Define your RST pin
    constexpr int dmaChannel = 1;
}
