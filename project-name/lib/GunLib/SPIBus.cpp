// SPIBus.cpp
#include "SPIBus.hpp"
#include <esp_log.h>
#include "Macros.hpp"
#include <stdexcept>
#include "MutexLockGuard.hpp"

SPIBus::SPIBus(spi_host_device_t hostID) : SPIHostHandle_(hostID) {
    spi_bus_config_t buscfg = SPIBus::buildBusConfig();

    spiMutex_ = xSemaphoreCreateRecursiveMutex();
    if (spiMutex_ == nullptr) {
        throw std::runtime_error("Failed to create SPI mutex");
    }

    if (spi_bus_initialize(SPIHostHandle_, &buscfg, SPIConfig::dmaChannel) != ESP_OK) {
        ESP_LOGE("SPIBus", "Failed to initialize SPI bus");
        throw std::runtime_error("Failed to initialize SPI bus");
    }
}

SPIBus::~SPIBus() {
    // Clean up all attached devices before freeing the bus
    cleanupDevices();

    // Free the SPI bus
    if (spi_bus_free(SPIHostHandle_) != ESP_OK) {
        ESP_LOGE("SPIBus", "Failed to free SPI bus");
    }
}

SPIDevice SPIBus::addDeviceToBus(int csPin, int spiClockSpeedHz) {
    MutexLockGuard lock(spiMutex_);

    spi_device_interface_config_t deviceConfig = SPIDevice::buildDeviceConfig(csPin, spiClockSpeedHz);
    spi_device_handle_t deviceHandle;

    if (spi_bus_add_device(SPIHostHandle_, &deviceConfig, &deviceHandle) != ESP_OK) {
        ESP_LOGE("SPIBus", "Failed to add device to SPI bus");
        throw std::runtime_error("Failed to add device to SPI bus");
    }

    // Store the device handle for later cleanup
    spiDevices_.push_back(deviceHandle);

    // Return the newly added SPIDevice instance
    return SPIDevice(deviceHandle);
}

spi_bus_config_t SPIBus::buildBusConfig() {
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

void SPIBus::cleanupDevices() {
    MutexLockGuard lock(spiMutex_);

    for (auto& device : spiDevices_) {
        if (spi_bus_remove_device(device) != ESP_OK) {
            ESP_LOGE("SPIBus", "Failed to remove SPI device");
        }
    }
    spiDevices_.clear();  // Clear the vector to avoid stale handles
}
