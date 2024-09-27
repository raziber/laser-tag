#include "SPIDevice.hpp"
#include "RFIDConfig.hpp"

std::optional<SPIDevice> SPIDevice::makeSPIDevice(SPIBus& bus){
    spi_device_interface_config_t deviceConfig = SPIDevice::buildDeviceConfig();

    bus.addDeviceToBus();
    if(sis_not_work){
        LOGERROR();
        return std::nullopt;
    }

    return std::make_optional<SPIDevice>();
}

spi_device_interface_config_t SPIDevice::buildDeviceConfig(){
    spi_device_interface_config_t deviceConfig = {};
    deviceConfig.clock_speed_hz = RFIDConfig::spiClockSpeedHz;
    deviceConfig.mode = 0;
    deviceConfig.spics_io_num = RFIDConfig::csPin;
    deviceConfig.queue_size = 1;

    return deviceConfig;
}
