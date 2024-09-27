#pragma once

#include <optional>
#include "SPIDevice.hpp"

class SPIBus{
public:
    static std::optional<SPIBus> make(spi_host_device_t hostID);

    bool addDeviceToBus(SPIDevice device);
private:
    SPIBus(spi_host_device_t hostID);
    ~SPIBus();

    static spi_bus_config_t buildBusConfig();

    spi_host_device_t SPIHostHandle_;
};
