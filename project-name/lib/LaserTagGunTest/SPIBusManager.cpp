#include "SPIBusManager.hpp"
#include "SPIConfig.hpp"
#include <driver/spi_master.h>
#include <esp_log.h>

esp_err_t SPIBusManager::initializeBus() {
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = SPIConfig::mosiPin;
    buscfg.miso_io_num = SPIConfig::misoPin;
    buscfg.sclk_io_num = SPIConfig::sckPin;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = 4096;
    buscfg.flags = SPICOMMON_BUSFLAG_MASTER;

    esp_err_t ret = spi_bus_initialize(SPIConfig::spiHost, &buscfg, SPIConfig::dmaChannel);
    if (ret != ESP_OK) {
        ESP_LOGE("SPIBusManager", "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
    }
    return ret;
}