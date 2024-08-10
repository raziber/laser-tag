#pragma once

#include "irProtocols.h"
#include "configuration.h"

namespace irSettings {
    constexpr int NUM_SENSORS = irSettings::sensorPins.size();
    constexpr int NUM_SHOOTERS = irSettings::shooterPins.size();

    inline constexpr ProtocolSettings irProtocolSettings = getProtocolSettings(irSettings::selectedProtocol);
}
