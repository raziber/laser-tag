#ifndef CONFIGURATION_BACKEND_H
#define CONFIGURATION_BACKEND_H

constexpr int NUM_SENSORS = irSettings::SENSOR_PINS.size();
constexpr int NUM_SHOOTERS = irSettings::SHOOTER_PINS.size();

ProtocolSettings irProtocolSettings = getProtocolSettings(selectedProtocol);

#endif // CONFIGURATION_BACKEND_H
