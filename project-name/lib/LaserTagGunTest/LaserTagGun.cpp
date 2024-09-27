#include "LaserTagGun.hpp"
#include"SPIConfig.hpp"
#include "ErrorStates.hpp"
#include "Macros.hpp"

std::optional<LaserTagGun> LaserTagGun::make(){
    SPIBus spiBus = MAKE_WITH_ARGS(SPIBus, SPIConfig::spiHost);

    return std::make_optional<LaserTagGun>(/*args*/);
}

// Constructor
LaserTagGun::LaserTagGun(gpio_num_t irLedPin, gpio_num_t buttonPin, IRProtocol protocol, std::unique_ptr<RFIDReader> rfidReader)
    : irLedPin_(irLedPin), buttonPin_(buttonPin), protocol_(protocol),
      irTransmitter_(irLedPin_, protocol_), buttonHandler_(buttonPin_),
      rfidReader_(std::move(rfidReader)) // Move the RFIDReader unique_ptr
{
    SPIBus spiBus_ = MAKE_WITH_ARGS(SPIBus, SPIConfig::spiHost);
    SPIDevice rfid_ = MAKE_WITH_ARGS(SPIDevice, spiBus_);

    spiBus_.addDeviceToBus(RFIDConfig, rfid_);
}

// Destructor
LaserTagGun::~LaserTagGun() {
    if (playerMutex_ != nullptr) {
        vSemaphoreDelete(playerMutex_);
        playerMutex_ = nullptr;
    }
}

// Start the system
esp_err_t LaserTagGun::start() {
    esp_err_t ret;

    // Create mutex
    playerMutex_ = xSemaphoreCreateMutex();
    if (playerMutex_ == nullptr) {
        ESP_LOGE("LaserTagGun", "Failed to create player mutex");
        // crirital_state();
    }

    // Initialize ButtonHandler
    ret = buttonHandler_.initialize();
    if (ret != ESP_OK) {
        ESP_LOGE("LaserTagGun", "Failed to initialize ButtonHandler");
        return ret;
    }

    // Initialize RFIDReader
    if (!rfidReader_) {
        ESP_LOGE("LaserTagGun", "rfidReader_ is null");
        return ESP_FAIL;
    }

    ret = rfidReader_->initialize();
    if (ret != ESP_OK) {
        ESP_LOGE("LaserTagGun", "Failed to initialize RFIDReader");
        return ret;
    }

    // Create tasks
    xTaskCreate(&LaserTagGun::rfidTask, "RFIDTask", 4096, this, 5, &rfidTaskHandle_);
    xTaskCreate(&LaserTagGun::buttonTask, "ButtonTask", 2048, this, 5, &buttonTaskHandle_);

    return ESP_OK;
}

// RFID Task
void LaserTagGun::rfidTask(void* arg) {
    LaserTagGun* gun = static_cast<LaserTagGun*>(arg);
    while (true) {
        gun->rfidReader_->scan();
        std::string tagId = gun->rfidReader_->getLastTagId();
        if (!tagId.empty()) {
            gun->handleRFIDEvent(tagId);
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // Adjust delay as needed
    }
}

// Button Task
void LaserTagGun::buttonTask(void* arg) {
    LaserTagGun* gun = static_cast<LaserTagGun*>(arg);
    while (true) {
        gun->buttonHandler_.monitor();
        if (gun->buttonHandler_.isButtonPressed()) {
            gun->handleButtonEvent();
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Adjust delay as needed
    }
}

// Handle RFID Event
void LaserTagGun::handleRFIDEvent(const std::string& tagId) {
    if (xSemaphoreTake(playerMutex_, portMAX_DELAY) == pdTRUE) {
        currentPlayer_.setId(tagId);
        personalCode_ = currentPlayer_.getPersonalCode();
        xSemaphoreGive(playerMutex_);
        ESP_LOGI("LaserTagGun", "Player ID updated: %s", tagId.c_str());
    }
}

// Handle Button Event
void LaserTagGun::handleButtonEvent() {
    fire();
}

// Fire function
void LaserTagGun::fire() {
    uint32_t code;
    if (xSemaphoreTake(playerMutex_, portMAX_DELAY) == pdTRUE) {
        code = personalCode_;
        xSemaphoreGive(playerMutex_);
    } else {
        ESP_LOGE("LaserTagGun", "Failed to acquire player mutex");
        return;
    }

    if (code != 0) {
        if (irTransmitter_.transmit(code) == ESP_OK) {
            ESP_LOGI("LaserTagGun", "Fired code: 0x%X", code);
        } else {
            ESP_LOGE("LaserTagGun", "Failed to transmit code");
        }
    } else {
        ESP_LOGW("LaserTagGun", "No player code set. Cannot fire.");
    }
}
