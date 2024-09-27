#include "SPIDevice.hpp"
#include <esp_log.h>
#include "Macros.hpp"

SPIDevice::SPIDevice(spi_device_handle_t deviceHandle) : deviceHandle_(deviceHandle){}

spi_device_interface_config_t SPIDevice::buildDeviceConfig(int csPin, int spiClockSpeedHz){
    spi_device_interface_config_t deviceConfig = {};
    deviceConfig.clock_speed_hz = spiClockSpeedHz;
    deviceConfig.mode = 0;
    deviceConfig.spics_io_num = csPin;
    deviceConfig.queue_size = 1;

    return deviceConfig;
}

bool SPIDevice::remove(){
    TRY_BOOL(spi_bus_remove_device, "SPIDevice", "Failed to remove SPI device", deviceHandle_);

    return true;
}
