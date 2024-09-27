#include "SPIBus.hpp"
#include "SPIConfig.hpp"
#include <esp_log.h>

SPIBus::SPIBus(spi_host_device_t hostID) : SPIHostHandle_(hostID){}

SPIBus::~SPIBus(){
    esp_err_t ret = spi_bus_free(SPIHostHandle_);
    if (ret != ESP_OK) {
        ESP_LOGE("SPIBus", "Failed to free SPI bus: %s", esp_err_to_name(ret));
    }
}

std::optional<SPIBus> SPIBus::makeSPIBus(spi_host_device_t hostID){
    spi_bus_config_t buscfg = SPIBus::buildBusConfig();

    esp_err_t ret = spi_bus_initialize(hostID, &buscfg, SPIConfig::dmaChannel);
    if (ret != ESP_OK) {
        ESP_LOGE("SPIBus", "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return std::nullopt;
    }

    return std::make_optional<SPIBus>();
}

bool SPIBus::addDeviceToBus(spi_device_interface_config_t *deviceConfig, spi_device_handle_t *deviceHandle){
    esp_err_t ret = spi_bus_add_device(SPIHostHandle_, deviceConfig, deviceHandle);
    if (ret != ESP_OK) {
        ESP_LOGE("SPIBus", "Failed to add device to SPI bus: %s", esp_err_to_name(ret));
        return false;
    }

    return true;
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
