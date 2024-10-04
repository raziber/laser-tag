#pragma once

#include <optional>
#include "SPIDevice.hpp"

class SPIBus{
public:
    static std::optional<SPIBus> make(spi_host_device_t hostID);
    ~SPIBus();

    std::optional<SPIDevice> addDeviceToBus(int csPin, int spiClockSpeedHz);
private:
    SPIBus(spi_host_device_t hostID);

    static spi_bus_config_t buildBusConfig();

    spi_host_device_t SPIHostHandle_;
};
