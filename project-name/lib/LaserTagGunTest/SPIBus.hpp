#pragma once

#include <optional>

class SPIBus{
public:
    SPIBus(spi_host_device_t hostID);
    ~SPIBus();

    static std::optional<SPIBus> makeSPIBus(spi_host_device_t hostID);

    bool addDeviceToBus(spi_device_interface_config_t *deviceConfig, spi_device_handle_t deviceHandle);
private:
    static spi_bus_config_t buildBusConfig();

    spi_host_device_t SPIHostHandle_;
};
