#pragma once

#include "configuration.h"

namespace IRConfig {
    static constexpr int NUM_SENSORS = IRConfig::sensorPins.size();
    static constexpr int NUM_SHOOTERS = IRConfig::shooterPins.size();
}
