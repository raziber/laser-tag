#pragma once

#include <Arduino.h>
#include "IRProtocolEnum.hpp"

namespace LaserTagGunConstants {
    constexpr gpio_num_t irLedPin = GPIO_NUM_4;
    constexpr gpio_num_t buttonPin = GPIO_NUM_5;
    constexpr gpio_num_t ledPin = GPIO_NUM_2;
    constexpr gpio_num_t buzzerPin = GPIO_NUM_15;
    constexpr IRProtocol protocol = IRProtocol::NEC;
}
