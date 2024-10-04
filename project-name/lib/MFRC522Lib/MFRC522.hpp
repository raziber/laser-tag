#pragma once

#include <cstdint>
#include <cstring>
#include <array>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_err.h"
#include "SPIBus.hpp"

namespace MFRC522Constants {

    // SPI Pin Definitions
    static constexpr gpio_num_t defaultMisoPin_ = GPIO_NUM_25;
    static constexpr gpio_num_t defaultMosiPin_ = GPIO_NUM_23;
    static constexpr gpio_num_t defaultClkPin_  = GPIO_NUM_19;
    static constexpr gpio_num_t defaultCsPin_   = GPIO_NUM_22;
    static constexpr gpio_num_t defaultRstPin_  = GPIO_NUM_14;

    // MFRC522 Registers
    enum class Register : uint8_t {
        CommandRegister             = 0x01,
        CommInterruptEnableRegister = 0x02,
        DivInterruptEnableRegister  = 0x03,
        CommInterruptRequestRegister= 0x04,
        DivInterruptRequestRegister = 0x05,
        ErrorRegister               = 0x06,
        Status1Register             = 0x07,
        Status2Register             = 0x08,
        FIFODataRegister            = 0x09,
        FIFOLevelRegister           = 0x0A,
        WaterLevelRegister          = 0x0B,
        ControlRegister             = 0x0C,
        BitFramingRegister          = 0x0D,
        CollisionRegister           = 0x0E,
        // Page 1: Command
        ModeRegister                = 0x11,
        TxModeRegister              = 0x12,
        RxModeRegister              = 0x13,
        TxControlRegister           = 0x14,
        TxASKRegister               = 0x15,
        RxSelRegister               = 0x17,
        RFConfigurationRegister     = 0x26,
        GsNRegister                 = 0x27,
        CWGsPRegister               = 0x28,
        ModGsPRegister              = 0x29,
        TModeRegister               = 0x2A,
        TPrescalerRegister          = 0x2B,
        TReloadHighRegister         = 0x2C,
        TReloadLowRegister          = 0x2D,
        CRCResultHighRegister       = 0x21,
        CRCResultLowRegister        = 0x22,
        VersionRegister             = 0x37,
    };

    // PCD Commands
    enum class Command : uint8_t {
        Idle                 = 0x00,
        Memory               = 0x01,
        GenerateRandomID     = 0x02,
        CalculateCRC         = 0x03,
        Transmit             = 0x04,
        NoCommandChange      = 0x07,
        Receive              = 0x08,
        Transceive           = 0x0C,
        MIFAREAuthenticate   = 0x0E,
        SoftReset            = 0x0F
    };

    // PICC Commands
    enum class PICCCommand : uint8_t {
        RequestA              = 0x26,
        WakeUpA               = 0x52,
        AntiCollisionCL1      = 0x93,
        AntiCollisionCL2      = 0x95,
        AntiCollisionCL3      = 0x97,
        SelectCascadeLevel1   = 0x93,
        SelectCascadeLevel2   = 0x95,
        SelectCascadeLevel3   = 0x97,
        CascadeTag            = 0x88,
        HaltA                 = 0x50,
        AuthenticateKeyA      = 0x60,
        AuthenticateKeyB      = 0x61,
        Read                  = 0x30,
        Write                 = 0xA0,
        Increment             = 0xC1,
        Decrement             = 0xC0,
        Restore               = 0xC2,
        Transfer              = 0xB0,
        // Add other commands as needed
    };

    // MIFARE Key Sizes
    static constexpr size_t mifareKeySize_ = 6;

    // Other constants
    static constexpr size_t maxUidLength_ = 10;
    static constexpr int spiClockSpeedHz_ = 5000000; // 5 MHz
}

class MFRC522 {
public:
    // Constructor and Destructor
    MFRC522(spi_host_device_t spiHost = HSPI_HOST,
            SPIBusManager spiBus,
            gpio_num_t misoPin = MFRC522Constants::defaultMisoPin_,
            gpio_num_t mosiPin = MFRC522Constants::defaultMosiPin_,
            gpio_num_t clkPin  = MFRC522Constants::defaultClkPin_,
            gpio_num_t csPin   = MFRC522Constants::defaultCsPin_,
            gpio_num_t rstPin  = MFRC522Constants::defaultRstPin_);
    ~MFRC522();

    // Initialization
    esp_err_t initialize();

    // Card detection and reading
    esp_err_t isNewCardPresent(bool& cardPresent);
    esp_err_t readCardSerial(std::array<uint8_t, MFRC522Constants::maxUidLength_>& uid, uint8_t& uidSize);

    // Authentication and data access
    esp_err_t authenticate(uint8_t blockAddr, uint8_t keyType, const uint8_t* key);
    esp_err_t stopCrypto();

    // Reading and writing data blocks
    esp_err_t readBlock(uint8_t blockAddr, uint8_t* buffer, uint8_t* bufferSize);
    esp_err_t writeBlock(uint8_t blockAddr, const uint8_t* buffer, uint8_t bufferSize);

    // Getters
    gpio_num_t getRstPin() const;

private:
    // SPI communication
    SPIBus spiBus_;
    spi_device_handle_t spiHandle_;
    SemaphoreHandle_t spiMutex_;

    // GPIO Pins
    gpio_num_t misoPin_;
    gpio_num_t mosiPin_;
    gpio_num_t clkPin_;
    gpio_num_t csPin_;
    gpio_num_t rstPin_;

    // Internal UID storage
    std::array<uint8_t, MFRC522Constants::maxUidLength_> uid_;
    uint8_t uidSize_;

    // Low-level register access
    esp_err_t writeRegister(MFRC522Constants::Register reg, uint8_t value);
    esp_err_t readRegister(MFRC522Constants::Register reg, uint8_t& value);
    esp_err_t setRegisterBitMask(MFRC522Constants::Register reg, uint8_t mask);
    esp_err_t clearRegisterBitMask(MFRC522Constants::Register reg, uint8_t mask);

    // High-level functions
    esp_err_t communicateWithPICC(MFRC522Constants::Command command, uint8_t waitIRq,
                                  const uint8_t* sendData, uint8_t sendLen,
                                  uint8_t* backData, uint8_t* backLen,
                                  uint8_t* validBits = nullptr, uint8_t rxAlign = 0,
                                  bool checkCRC = false);
    esp_err_t transceiveData(const uint8_t* sendData, uint8_t sendLen,
                             uint8_t* backData, uint8_t* backLen,
                             uint8_t* validBits = nullptr, uint8_t rxAlign = 0,
                             bool checkCRC = false);
    esp_err_t requestA(uint8_t* bufferATQA, uint8_t* bufferSize);
    esp_err_t selectCard();

    // Utility functions
    esp_err_t reset();
    esp_err_t antennaOn();
    esp_err_t antennaOff();
    esp_err_t calculateCRC(const uint8_t* data, uint8_t length, uint8_t* result);

    // Initialization helpers
    esp_err_t configureTimer();
    esp_err_t configureAntenna();
};
