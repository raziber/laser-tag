#pragma once

#include <optional>
#include <driver/spi_master.h>
#include "SPIBus.hpp"

class SPIDevice{
public:
    SPIDevice(spi_device_handle_t deviceHandle);
    ~SPIDevice() = default;

    static spi_device_interface_config_t buildDeviceConfig(int csPin, int spiClockSpeedHz);
private:
    spi_device_handle_t deviceHandle_;
    spi_device_interface_config_t deviceConfig_;
};
