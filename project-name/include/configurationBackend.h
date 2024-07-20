#pragma once

#include "irProtocols.h"
#include "configuration.h"

namespace irSettings {
    constexpr int NUM_SENSORS = irSettings::SENSOR_PINS.size();
    constexpr int NUM_SHOOTERS = irSettings::SHOOTER_PINS.size();

    inline constexpr ProtocolSettings irProtocolSettings = getProtocolSettings(irSettings::selectedProtocol);
}
