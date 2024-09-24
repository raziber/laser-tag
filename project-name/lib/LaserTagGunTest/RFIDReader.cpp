#include "RFIDReader.hpp"
#include <esp_log.h>

RFIDReader::RFIDReader(std::unique_ptr<MFRC522> mfrc522)
    : mfrc522_(std::move(mfrc522)), lastTagId_(""), tagMutex_(nullptr) {
    tagMutex_ = xSemaphoreCreateMutex();
    if (tagMutex_ == nullptr) {
        ESP_LOGE("RFIDReader", "Failed to create tag mutex");
    }
}

RFIDReader::~RFIDReader() {
    if (tagMutex_ != nullptr) {
        vSemaphoreDelete(tagMutex_);
        tagMutex_ = nullptr;
    }
}

esp_err_t RFIDReader::initialize() {
    esp_err_t ret = initHardware();
    if (ret != ESP_OK) {
        ESP_LOGE("RFIDReader", "Failed to initialize hardware");
        return ret;
    }

    ret = mfrc522_->initialize();
    if (ret != ESP_OK) {
        ESP_LOGE("RFIDReader", "Failed to initialize MFRC522");
        return ret;
    }

    return ESP_OK;
}

esp_err_t RFIDReader::initHardware() {
    // Initialize RST pin
    gpio_num_t rstPin = mfrc522_->getRstPin();
    esp_err_t ret = gpio_set_direction(rstPin, GPIO_MODE_OUTPUT);
    if (ret != ESP_OK) {
        ESP_LOGE("RFIDReader", "Failed to set RST pin direction");
        return ret;
    }

    // Initialize SPI bus if not already initialized
    // (Assuming mfrc522_ handles SPI initialization)
    // If SPI initialization is separate, do it here

    return ESP_OK;
}

void RFIDReader::scan() {
    // Existing scan logic
    bool cardPresent = false;
    esp_err_t ret = mfrc522_->isNewCardPresent(cardPresent);
    if (ret != ESP_OK || !cardPresent) {
        return;
    }

    std::array<uint8_t, MFRC522Constants::maxUidLength_> uid;
    uint8_t uidSize = 0;
    ret = mfrc522_->readCardSerial(uid, uidSize);
    if (ret != ESP_OK) {
        ESP_LOGE("RFIDReader", "Failed to read card serial");
        return;
    }

    // Convert UID to string
    std::string tagId;
    for (uint8_t i = 0; i < uidSize; i++) {
        char buffer[3];
        snprintf(buffer, sizeof(buffer), "%02X", uid[i]);
        tagId += buffer;
    }

    // Update lastTagId_ safely
    if (xSemaphoreTake(tagMutex_, portMAX_DELAY) == pdTRUE) {
        lastTagId_ = tagId;
        xSemaphoreGive(tagMutex_);
    }
}

std::string RFIDReader::getLastTagId() {
    std::string tagId;
    if (xSemaphoreTake(tagMutex_, portMAX_DELAY) == pdTRUE) {
        tagId = lastTagId_;
        lastTagId_.clear(); // Clear after reading
        xSemaphoreGive(tagMutex_);
    }
    return tagId;
}
