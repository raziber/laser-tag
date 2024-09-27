#include "SPIBus.hpp"
#include "SPIConfig.hpp"
#include <esp_log.h>
#include "Macros.hpp"

SPIBus::SPIBus(spi_host_device_t hostID) : SPIHostHandle_(hostID){}

SPIBus::~SPIBus(){
    // first remove all devices somehow
    TRY_VOID(spi_bus_free, "SPIBus", "Failed to free SPI bus", SPIHostHandle_);
}

std::optional<SPIBus> SPIBus::make(spi_host_device_t hostID){
    spi_bus_config_t buscfg = SPIBus::buildBusConfig();

    TRY_OPTIONAL(spi_bus_initialize, "SPIBus", "Failed to initialize SPI bus", hostID, &buscfg, SPIConfig::dmaChannel);
    return std::make_optional<SPIBus>(hostID);
}

std::optional<SPIDevice> SPIBus::addDeviceToBus(int csPin, int spiClockSpeedHz){
    spi_device_handle_t deviceHandle;
    spi_device_interface_config_t deviceConfig = SPIDevice::buildDeviceConfig(csPin, spiClockSpeedHz);

    TRY_OPTIONAL(spi_bus_add_device, "SPIBus", "Failed to add device to SPI bus", SPIHostHandle_, &deviceConfig, &deviceHandle);
    return std::make_optional<SPIDevice>(deviceHandle);
}

spi_bus_config_t SPIBus::buildBusConfig(){
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = SPIConfig::mosiPin;
    buscfg.miso_io_num = SPIConfig::misoPin;
    buscfg.sclk_io_num = SPIConfig::sckPin;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = 4096;
    buscfg.flags = SPICOMMON_BUSFLAG_MASTER;

    return buscfg;
}
