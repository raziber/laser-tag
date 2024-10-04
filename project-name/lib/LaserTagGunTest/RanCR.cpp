// Note: I have a lot more CR comments then what I am giving right now, this is to get started and deal with the most
// pressing issues.
#pragma once

#include <optional>

class SPIBus{
public:
    SPIBus(spi_host_device_t hostID); // CR: should be private (ask me why if you are not sure)
    ~SPIBus();

    static std::optional<SPIBus> makeSPIBus(spi_host_device_t hostID); // CR: call this make so that to create an in stance you just need to call SPIBus::make and not SPIBus::makeSPIBus

    bool addDeviceToBus(spi_device_interface_config_t *deviceConfig, spi_device_handle_t deviceHandle);
private:
    static spi_bus_config_t buildBusConfig();

    spi_host_device_t SPIHostHandle_;
};

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

    return std::make_optional<SPIBus>(); // CR:: don't forget to pass the hostID
}

bool SPIBus::addDeviceToBus(spi_device_interface_config_t *deviceConfig, spi_device_handle_t *deviceHandle){
    /*
     * CR: This is a tricky situation that is not very common (this is a harder case then usual)
     * You have a function (addDeviceToBus) that wraps spi_bus_add_device that takes not 1 but 2! handles.
     * As we talked about, we want each handle to be a private member of some class however it is not stricly possible
     * here because spi_bus_add_device needs access to 2 handles. (if it was in SPIBus it wouldn't have access the
     * SPIDevice handle and if it was in SPIDevice it wouldn't have access to the SPIBus handle)
     * There are various solutions to this problem but for now we will do a simple one (that we will improve on later)
     * Change this function to accept SPIDevice and make SPIDevice and SPIBus classes friends (read about it)
     * that way you will have access here to the private deviceHandle and deviceConfig.
     */
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

#pragma once

#include <optional>
#include <driver/spi_master.h>
#include "SPIBus.hpp"

class SPIDevice{
public:
    SPIDevice() = default;  // CR: should be private (ask me why if you are not sure)
    ~SPIDevice() = default;

    static std::optional<SPIDevice> makeSPIDevice(SPIBus& bus);
    spi_device_interface_config_t buildDeviceConfig();
private:
    spi_device_handle_t SPIDeviceHandle_;
};

#include "SPIDevice.hpp"
#include "RFIDConfig.hpp"

std::optional<SPIDevice> SPIDevice::makeSPIDevice(SPIBus& bus){
    spi_device_interface_config_t deviceConfig = SPIDevice::buildDeviceConfig();
    // CR: I guess in the future you will write here the code that will create the SPIDeviceHandle_
    bus.addDeviceToBus(); // CR: remove the device when SPIDevice goes out of scope.
    if(sis_not_work){
        LOGERROR();
        return std::nullopt;
    }

    return std::make_optional<SPIDevice>();
}

spi_device_interface_config_t SPIDevice::buildDeviceConfig(){
    spi_device_interface_config_t deviceConfig = {};
    deviceConfig.clock_speed_hz = RFIDConfig::spiClockSpeedHz; // CR: there is logic here of generic SPIDevice and of RFID. they should not be in the same class.
    deviceConfig.mode = 0;
    deviceConfig.spics_io_num = RFIDConfig::csPin;
    deviceConfig.queue_size = 1;

    return deviceConfig;
}