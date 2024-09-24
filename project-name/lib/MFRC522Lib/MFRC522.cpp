#include "MFRC522.hpp"

using namespace MFRC522Constants;

MFRC522::MFRC522(spi_host_device_t spiHost,
                 gpio_num_t misoPin,
                 gpio_num_t mosiPin,
                 gpio_num_t clkPin,
                 gpio_num_t csPin,
                 gpio_num_t rstPin)
    : spiHandle_(nullptr),
      spiMutex_(nullptr),
      misoPin_(misoPin),
      mosiPin_(mosiPin),
      clkPin_(clkPin),
      csPin_(csPin),
      rstPin_(rstPin),
      uidSize_(0)
{
    // Initialize UID array
    uid_.fill(0);
}

MFRC522::~MFRC522() {
    if (spiMutex_ != nullptr) {
        vSemaphoreDelete(spiMutex_);
    }
    if (spiHandle_ != nullptr) {
        spi_bus_remove_device(spiHandle_);
    }
    // Deinitialize SPI bus
    spi_bus_free(HSPI_HOST);
}

esp_err_t MFRC522::initialize() {
    esp_err_t ret;

    // Create SPI mutex
    spiMutex_ = xSemaphoreCreateMutex();
    if (spiMutex_ == nullptr) {
        ESP_LOGE("MFRC522", "Failed to create SPI mutex");
        return ESP_ERR_NO_MEM;
    }

    // Initialize RST pin
    ret = gpio_set_direction(rstPin_, GPIO_MODE_OUTPUT);
    if (ret != ESP_OK) {
        ESP_LOGE("MFRC522", "Failed to set RST pin direction");
        return ret;
    }

    // Initialize SPI bus
    spi_bus_config_t busConfig = {};
    busConfig.miso_io_num = misoPin_;
    busConfig.mosi_io_num = mosiPin_;
    busConfig.sclk_io_num = clkPin_;
    busConfig.quadwp_io_num = -1;
    busConfig.quadhd_io_num = -1;
    busConfig.max_transfer_sz = 0;

    ret = spi_bus_initialize(HSPI_HOST, &busConfig, 1);
    if (ret != ESP_OK) {
        ESP_LOGE("MFRC522", "Failed to initialize SPI bus");
        return ret;
    }

    // Add SPI device
    spi_device_interface_config_t deviceConfig = {};
    deviceConfig.clock_speed_hz = spiClockSpeedHz_;
    deviceConfig.mode = 0;
    deviceConfig.spics_io_num = csPin_;
    deviceConfig.queue_size = 1;

    ret = spi_bus_add_device(HSPI_HOST, &deviceConfig, &spiHandle_);
    if (ret != ESP_OK) {
        ESP_LOGE("MFRC522", "Failed to add SPI device");
        return ret;
    }

    // Reset the MFRC522
    ret = reset();
    if (ret != ESP_OK) {
        ESP_LOGE("MFRC522", "Failed to reset MFRC522");
        return ret;
    }

    // Configure the MFRC522
    ret = configureTimer();
    if (ret != ESP_OK) return ret;

    ret = configureAntenna();
    if (ret != ESP_OK) {
        ESP_LOGE("MFRC522", "Failed to configure antenna");
        return ret;
    }

    return ESP_OK;
}

esp_err_t MFRC522::reset() {
    esp_err_t ret;

    ret = gpio_set_level(rstPin_, 0);
    if (ret != ESP_OK) return ret;

    vTaskDelay(pdMS_TO_TICKS(50));

    ret = gpio_set_level(rstPin_, 1);
    if (ret != ESP_OK) return ret;

    vTaskDelay(pdMS_TO_TICKS(50));

    ret = writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::SoftReset));
    if (ret != ESP_OK) return ret;

    vTaskDelay(pdMS_TO_TICKS(50));

    return ESP_OK;
}

esp_err_t MFRC522::configureTimer() {
    esp_err_t ret;

    // Timer settings
    ret = writeRegister(Register::TModeRegister, 0x80);
    if (ret != ESP_OK) return ret;

    ret = writeRegister(Register::TPrescalerRegister, 0xA9);
    if (ret != ESP_OK) return ret;

    ret = writeRegister(Register::TReloadHighRegister, 0x03);
    if (ret != ESP_OK) return ret;

    ret = writeRegister(Register::TReloadLowRegister, 0xE8);
    if (ret != ESP_OK) return ret;

    ret = writeRegister(Register::TxASKRegister, 0x40);
    if (ret != ESP_OK) return ret;

    ret = writeRegister(Register::ModeRegister, 0x3D);
    if (ret != ESP_OK) return ret;

    return ESP_OK;
}

esp_err_t MFRC522::configureAntenna() {
    return antennaOn();
}

esp_err_t MFRC522::antennaOn() {
    uint8_t value;
    esp_err_t ret = readRegister(Register::TxControlRegister, value);
    if (ret != ESP_OK) return ret;

    if ((value & 0x03) != 0x03) {
        ret = writeRegister(Register::TxControlRegister, value | 0x03);
        if (ret != ESP_OK) return ret;
    }
    return ESP_OK;
}

esp_err_t MFRC522::antennaOff() {
    return clearRegisterBitMask(Register::TxControlRegister, 0x03);
}

esp_err_t MFRC522::isNewCardPresent(bool& cardPresent) {
    uint8_t bufferATQA[2] = {0};
    uint8_t bufferSize = sizeof(bufferATQA);

    esp_err_t ret = requestA(bufferATQA, &bufferSize);
    if (ret == ESP_OK || ret == ESP_ERR_INVALID_RESPONSE) {
        cardPresent = true;
        return ESP_OK;
    } else if (ret == ESP_ERR_TIMEOUT) {
        cardPresent = false;
        return ESP_OK;
    } else {
        ESP_LOGE("MFRC522", "Error checking for new card: %s", esp_err_to_name(ret));
        return ret;
    }
}

esp_err_t MFRC522::readCardSerial(std::array<uint8_t, maxUidLength_>& uid, uint8_t& uidSize) {
    esp_err_t ret = selectCard();
    if (ret != ESP_OK) {
        ESP_LOGE("MFRC522", "Failed to select card: %s", esp_err_to_name(ret));
        return ret;
    }
    uid = uid_;
    uidSize = uidSize_;
    return ESP_OK;
}

esp_err_t MFRC522::authenticate(uint8_t blockAddr, uint8_t keyType, const uint8_t* key) {
    uint8_t waitIRq = 0x10; // IdleIRq

    uint8_t sendData[12];
    sendData[0] = keyType; // Use PICCCommand::AuthenticateKeyA or AuthenticateKeyB
    sendData[1] = blockAddr;

    // Copy the key
    memcpy(&sendData[2], key, mifareKeySize_);

    // Copy the UID (first 4 bytes)
    memcpy(&sendData[8], uid_.data(), 4);

    uint8_t backLen = 0;

    esp_err_t ret = communicateWithPICC(Command::MIFAREAuthenticate, waitIRq, sendData, sizeof(sendData), nullptr, &backLen);
    if (ret != ESP_OK) {
        ESP_LOGE("MFRC522", "Authentication failed: %s", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t MFRC522::stopCrypto() {
    // Clear MFCrypto1On bit
    return clearRegisterBitMask(Register::Status2Register, 0x08);
}

esp_err_t MFRC522::readBlock(uint8_t blockAddr, uint8_t* buffer, uint8_t* bufferSize) {
    if (buffer == nullptr || bufferSize == nullptr || *bufferSize < 18) {
        return ESP_ERR_INVALID_ARG;
    }

    buffer[0] = static_cast<uint8_t>(PICCCommand::Read);
    buffer[1] = blockAddr;

    uint8_t crc[2];
    esp_err_t ret = calculateCRC(buffer, 2, crc);
    if (ret != ESP_OK) return ret;

    buffer[2] = crc[0];
    buffer[3] = crc[1];

    uint8_t validBits = 0;
    ret = transceiveData(buffer, 4, buffer, bufferSize, &validBits, 0, true);
    if (ret != ESP_OK) {
        ESP_LOGE("MFRC522", "Read block failed: %s", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t MFRC522::writeBlock(uint8_t blockAddr, const uint8_t* buffer, uint8_t bufferSize) {
    if (buffer == nullptr || bufferSize != 16) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t cmdBuffer[4];
    cmdBuffer[0] = static_cast<uint8_t>(PICCCommand::Write);
    cmdBuffer[1] = blockAddr;

    uint8_t crc[2];
    esp_err_t ret = calculateCRC(cmdBuffer, 2, crc);
    if (ret != ESP_OK) return ret;

    cmdBuffer[2] = crc[0];
    cmdBuffer[3] = crc[1];

    uint8_t backData[1];
    uint8_t backLen = sizeof(backData);

    ret = transceiveData(cmdBuffer, 4, backData, &backLen, nullptr, 0, true);
    if (ret != ESP_OK || backLen != 1 || (backData[0] & 0x0F) != 0x0A) {
        ESP_LOGE("MFRC522", "Write command failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // Send data block
    uint8_t dataBuffer[18];
    memcpy(dataBuffer, buffer, 16);

    ret = calculateCRC(buffer, 16, crc);
    if (ret != ESP_OK) return ret;

    dataBuffer[16] = crc[0];
    dataBuffer[17] = crc[1];

    backLen = sizeof(backData);
    ret = transceiveData(dataBuffer, 18, backData, &backLen, nullptr, 0, true);
    if (ret != ESP_OK || backLen != 1 || (backData[0] & 0x0F) != 0x0A) {
        ESP_LOGE("MFRC522", "Writing data block failed: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

// Low-level SPI write
esp_err_t MFRC522::writeRegister(Register reg, uint8_t value) {
    if (xSemaphoreTake(spiMutex_, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE("MFRC522", "Failed to take SPI mutex for writing");
        return ESP_ERR_TIMEOUT;
    }

    uint8_t address = (static_cast<uint8_t>(reg) << 1) & 0x7E;
    uint8_t data[2] = { address, value };

    spi_transaction_t transaction = {};
    transaction.length = 8 * sizeof(data);
    transaction.tx_buffer = data;

    esp_err_t ret = spi_device_transmit(spiHandle_, &transaction);
    xSemaphoreGive(spiMutex_);

    if (ret != ESP_OK) {
        ESP_LOGE("MFRC522", "SPI write failed: %s", esp_err_to_name(ret));
    }
    return ret;
}

// Low-level SPI read
esp_err_t MFRC522::readRegister(Register reg, uint8_t& value) {
    if (xSemaphoreTake(spiMutex_, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE("MFRC522", "Failed to take SPI mutex for reading");
        return ESP_ERR_TIMEOUT;
    }

    uint8_t address = ((static_cast<uint8_t>(reg) << 1) & 0x7E) | 0x80;
    uint8_t data[2] = { address, 0 };

    spi_transaction_t transaction = {};
    transaction.length = 8 * sizeof(data);
    transaction.tx_buffer = data;
    transaction.rx_buffer = data;

    esp_err_t ret = spi_device_transmit(spiHandle_, &transaction);
    xSemaphoreGive(spiMutex_);

    if (ret != ESP_OK) {
        ESP_LOGE("MFRC522", "SPI read failed: %s", esp_err_to_name(ret));
        return ret;
    }

    value = data[1];
    return ret;
}

esp_err_t MFRC522::setRegisterBitMask(Register reg, uint8_t mask) {
    uint8_t value;
    esp_err_t ret = readRegister(reg, value);
    if (ret != ESP_OK) return ret;

    value |= mask;
    return writeRegister(reg, value);
}

esp_err_t MFRC522::clearRegisterBitMask(Register reg, uint8_t mask) {
    uint8_t value;
    esp_err_t ret = readRegister(reg, value);
    if (ret != ESP_OK) return ret;

    value &= ~mask;
    return writeRegister(reg, value);
}

// Request command
esp_err_t MFRC522::requestA(uint8_t* bufferATQA, uint8_t* bufferSize) {
    uint8_t validBits = 7;
    esp_err_t ret;

    ret = clearRegisterBitMask(Register::CollisionRegister, 0x80); // ValuesAfterColl=1 => Bits received after collision are cleared.
    if (ret != ESP_OK) return ret;

    uint8_t command = static_cast<uint8_t>(PICCCommand::RequestA);
    ret = transceiveData(&command, 1, bufferATQA, bufferSize, &validBits);
    if (ret != ESP_OK) return ret;

    if (*bufferSize != 2 || validBits != 0) {
        return ESP_ERR_INVALID_RESPONSE;
    }
    return ESP_OK;
}

esp_err_t MFRC522::transceiveData(const uint8_t* sendData, uint8_t sendLen,
                                  uint8_t* backData, uint8_t* backLen,
                                  uint8_t* validBits, uint8_t rxAlign,
                                  bool checkCRC) {
    uint8_t waitIRq = 0x30; // RxIRq and IdleIRq
    return communicateWithPICC(Command::Transceive, waitIRq, sendData, sendLen,
                               backData, backLen, validBits, rxAlign, checkCRC);
}

esp_err_t MFRC522::communicateWithPICC(Command command, uint8_t waitIRq,
                                       const uint8_t* sendData, uint8_t sendLen,
                                       uint8_t* backData, uint8_t* backLen,
                                       uint8_t* validBits, uint8_t rxAlign,
                                       bool checkCRC) {
    esp_err_t ret;
    uint8_t n;
    uint8_t txLastBits = validBits ? *validBits : 0;
    uint8_t bitFraming = (rxAlign << 4) + txLastBits;

    ret = writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::Idle));
    if (ret != ESP_OK) return ret;

    ret = writeRegister(Register::CommInterruptRequestRegister, 0x7F); // Clear interrupt flags
    if (ret != ESP_OK) return ret;

    ret = writeRegister(Register::FIFOLevelRegister, 0x80); // Flush FIFO
    if (ret != ESP_OK) return ret;

    // Write data to FIFO
    for (uint8_t i = 0; i < sendLen; i++) {
        ret = writeRegister(Register::FIFODataRegister, sendData[i]);
        if (ret != ESP_OK) return ret;
    }

    ret = writeRegister(Register::BitFramingRegister, bitFraming);
    if (ret != ESP_OK) return ret;

    ret = writeRegister(Register::CommandRegister, static_cast<uint8_t>(command));
    if (ret != ESP_OK) return ret;

    if (command == Command::Transceive) {
        ret = setRegisterBitMask(Register::BitFramingRegister, 0x80); // StartSend=1
        if (ret != ESP_OK) return ret;
    }

    // Wait for the command to complete
    const TickType_t timeout = pdMS_TO_TICKS(50);
    TickType_t startTime = xTaskGetTickCount();

    while (true) {
        ret = readRegister(Register::CommInterruptRequestRegister, n);
        if (ret != ESP_OK) return ret;

        if (n & waitIRq) {
            break;
        }

        if (n & 0x01) { // Timer interrupt - no reply received
            return ESP_ERR_TIMEOUT;
        }

        if ((xTaskGetTickCount() - startTime) > timeout) {
            return ESP_ERR_TIMEOUT;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    // Check for errors
    uint8_t errorRegValue;
    ret = readRegister(Register::ErrorRegister, errorRegValue);
    if (ret != ESP_OK) return ret;

    if (errorRegValue & 0x13) { // BufferOvfl, ParityErr, ProtocolErr
        ESP_LOGE("MFRC522", "Communication error: 0x%02X", errorRegValue);
        return ESP_ERR_INVALID_RESPONSE;
    }

    // Read received data from FIFO
    if (backData && backLen) {
        ret = readRegister(Register::FIFOLevelRegister, n);
        if (ret != ESP_OK) return ret;

        if (n > *backLen) {
            return ESP_ERR_NO_MEM;
        }

        *backLen = n;

        for (uint8_t i = 0; i < n; i++) {
            ret = readRegister(Register::FIFODataRegister, backData[i]);
            if (ret != ESP_OK) return ret;
        }

        uint8_t _validBits;
        ret = readRegister(Register::ControlRegister, _validBits);
        if (ret != ESP_OK) return ret;

        _validBits &= 0x07;
        if (validBits) {
            *validBits = _validBits;
        }
    }

    // Check for collision
    if (errorRegValue & 0x08) { // CollErr
        return ESP_ERR_INVALID_RESPONSE;
    }

    return ESP_OK;
}

esp_err_t MFRC522::calculateCRC(const uint8_t* data, uint8_t length, uint8_t* result) {
    esp_err_t ret;

    ret = writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::Idle));
    if (ret != ESP_OK) return ret;

    ret = writeRegister(Register::DivInterruptRequestRegister, 0x04); // Clear CRCIRq
    if (ret != ESP_OK) return ret;

    ret = writeRegister(Register::FIFOLevelRegister, 0x80); // Flush FIFO
    if (ret != ESP_OK) return ret;

    // Write data to FIFO
    for (uint8_t i = 0; i < length; i++) {
        ret = writeRegister(Register::FIFODataRegister, data[i]);
        if (ret != ESP_OK) return ret;
    }

    ret = writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::CalculateCRC));
    if (ret != ESP_OK) return ret;

    // Wait for CRC calculation
    const TickType_t timeout = pdMS_TO_TICKS(50);
    TickType_t startTime = xTaskGetTickCount();
    uint8_t n;

    while (true) {
        ret = readRegister(Register::DivInterruptRequestRegister, n);
        if (ret != ESP_OK) return ret;

        if (n & 0x04) { // CRCIRq bit set
            break;
        }

        if ((xTaskGetTickCount() - startTime) > timeout) {
            return ESP_ERR_TIMEOUT;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    // Read CRC result
    ret = readRegister(Register::CRCResultLowRegister, result[0]);
    if (ret != ESP_OK) return ret;

    ret = readRegister(Register::CRCResultHighRegister, result[1]);
    if (ret != ESP_OK) return ret;

    ret = writeRegister(Register::CommandRegister, static_cast<uint8_t>(Command::Idle));
    if (ret != ESP_OK) return ret;

    return ESP_OK;
}

esp_err_t MFRC522::selectCard() {
    uint8_t buffer[9];
    uint8_t bufferSize = sizeof(buffer);
    esp_err_t ret;
    uint8_t cascadeLevel = 1;

    uidSize_ = 0;
    uid_.fill(0);

    while (cascadeLevel <= 3) {
        buffer[0] = static_cast<uint8_t>(static_cast<uint8_t>(PICCCommand::SelectCascadeLevel1) + 2 * (cascadeLevel - 1));
        buffer[1] = 0x20; // NVB - Number of valid bits: 32 bits = complete UID

        ret = transceiveData(buffer, 2, &buffer[2], &bufferSize, nullptr);
        if (ret != ESP_OK) return ret;

        if (bufferSize != 5) {
            return ESP_ERR_INVALID_RESPONSE;
        }

        // Check for Cascade Tag
        if (buffer[2] == static_cast<uint8_t>(PICCCommand::CascadeTag)) {
            // UID not complete, continue to next cascade level
            memcpy(&uid_[uidSize_], &buffer[3], 3);
            uidSize_ += 3;
            cascadeLevel++;
        } else {
            // UID complete
            memcpy(&uid_[uidSize_], &buffer[2], 4);
            uidSize_ += 4;
            break;
        }
    }

    return ESP_OK;
}
