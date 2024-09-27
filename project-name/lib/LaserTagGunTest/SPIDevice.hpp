#pragma once

#include <optional>
#include <driver/spi_master.h>
#include "SPIBus.hpp"

class SPIDevice{
public:
    SPIDevice() = default;
    ~SPIDevice() = default;

    static std::optional<SPIDevice> makeSPIDevice(SPIBus& bus);
    spi_device_interface_config_t buildDeviceConfig();
private:
    spi_device_handle_t SPIDeviceHandle_;
};
