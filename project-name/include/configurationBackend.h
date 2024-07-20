#pragma once

#include "irProtocols.h"
#include "configuration.h"

constexpr int NUM_SENSORS = irSettings::SENSOR_PINS.size();
constexpr int NUM_SHOOTERS = irSettings::SHOOTER_PINS.size();

ProtocolSettings irProtocolSettings = getProtocolSettings(selectedProtocol);

