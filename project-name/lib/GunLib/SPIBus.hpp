// SPIBus.hpp
#pragma once

#include "SPIConfig.hpp"
#include <driver/spi_master.h>
#include <vector>
#include "freertos/semphr.h"
#include "SPIDevice.hpp"

class SPIBus {
public:
    SPIBus(spi_host_device_t hostID);
    ~SPIBus();

    SPIDevice addDeviceToBus(int csPin, int spiClockSpeedHz);

private:
    spi_host_device_t SPIHostHandle_;
    std::vector<spi_device_handle_t> spiDevices_;  // Track all device handles for cleanup

    SemaphoreHandle_t spiMutex_;
    static spi_bus_config_t buildBusConfig();

    void cleanupDevices();  // Helper function to clean up all devices
};
