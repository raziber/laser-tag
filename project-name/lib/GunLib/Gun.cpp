// Gun.cpp
#include "Gun.hpp"
#include "SPIConfig.hpp"
#include "RFIDConfig.hpp"
#include "esp_log.h"

Gun::Gun()
    : spiBus_(nullptr),
      rfid_(nullptr),
      button_(nullptr),
      currentPlayerID_("") {

    try {
        spiBus_ = std::make_unique<SPIBus>(SPIConfig::spiHost);
    } catch (const std::exception& e){
        ESP_LOGE("Gun", "Exception during SPIBus initialization: %s", e.what());
        throw;
    }

    try {
        rfid_ = std::make_unique<RFID>(*spiBus_, RFIDConfig::csPin);
    } catch (const std::exception& e){
        ESP_LOGE("Gun", "Exception during RFID initialization: %s", e.what());
        throw;
    }

    static constexpr gpio_num_t BUTTON_GPIO = GPIO_NUM_30;
    static constexpr TickType_t BUTTON_DEBOUNCE_TIME_TICKS = pdMS_TO_TICKS(50);
    try {
        button_ = std::make_unique<Button>(BUTTON_GPIO, BUTTON_DEBOUNCE_TIME_TICKS);
    } catch (const std::exception& e) {
        ESP_LOGE("Gun", "Exception during Button initialization: %s", e.what());
        throw;
    }

    // Create queues for button press and RFID read
    const UBaseType_t BUTTON_QUEUE_SIZE = 25;  // Updated to match human ability to press trigger rapidly
    try {
        buttonQueue_ = std::make_unique<Queue<bool>>(BUTTON_QUEUE_SIZE);
    } catch (const std::exception& e){
        ESP_LOGE("Gun", "Exception during buttonQueue initialization: %s", e.what());
        throw;
    }

    const UBaseType_t RFID_QUEUE_SIZE = 10;
    try{
        rfidQueue_ = std::make_unique<Queue<std::string>>(RFID_QUEUE_SIZE);
    } catch (const std::exception& e){
        ESP_LOGE("Gun", "Exception during rfidQueue initialization: %s", e.what());
        throw;
    }
}

Gun::~Gun() = default;

void Gun::start() {
    // Start the main gun task
    const uint32_t GUN_TASK_STACK_SIZE = 4096;
    const UBaseType_t GUN_TASK_PRIORITY = 5;
    try {
        gunTask_ = std::make_unique<Task>("GunTask", GUN_TASK_STACK_SIZE, GUN_TASK_PRIORITY, [this] {
            try {
                gunTaskFunction();
            } catch (const std::exception& e) {
                ESP_LOGE("GunTask", "Exception in gunTaskFunction: %s", e.what());
            }
        });
    } catch (const std::runtime_error& e) {
        ESP_LOGE("Gun", "Failed to create gun task: %s", e.what());
    }

    // Start button monitoring task in Button class to detect button presses
    button_->startMonitoring(*buttonQueue_);

    // Start RFID monitoring task in RFID class to read player ID
    if (rfid_) {
        rfid_->startMonitoring(*rfidQueue_);
    } else {
        ESP_LOGE("Gun", "RFID instance is null, cannot start monitoring");
    }
}

void Gun::gunTaskFunction() {
    TickType_t lastWakeTime = xTaskGetTickCount();
    static constexpr uint32_t BUTTON_QUEUE_WAIT_MS = 10;
    static constexpr uint32_t RFID_QUEUE_WAIT_MS = 10;
    static constexpr uint32_t GUN_TASK_DELAY_MS = 10;

    while (!gunTask_->shouldStop()) {
        try {
            bool buttonPressed = false;
            // Check if the button was pressed
            if (buttonQueue_->receive(&buttonPressed, pdMS_TO_TICKS(BUTTON_QUEUE_WAIT_MS))) {
                if (buttonPressed) {
                    fire();  // Fire the gun if the button is pressed
                    ESP_LOGI("Gun", "Fired laser with player ID: %s", getPlayerID().c_str());
                }
            }

            std::string newPlayerID;
            // Check if a new player ID was read from the RFID
            if (rfidQueue_->receive(&newPlayerID, pdMS_TO_TICKS(RFID_QUEUE_WAIT_MS))) {
                currentPlayerID_ = newPlayerID;
                ESP_LOGI("Gun", "Updated player ID: %s", currentPlayerID_.c_str());
            }

            // Delay until the next iteration of the gun task
            vTaskDelay(pdMS_TO_TICKS(GUN_TASK_DELAY_MS));
        } catch (const std::exception& e) {
            ESP_LOGE("GunTask", "Exception in gunTaskFunction: %s", e.what());
            // Optionally decide to exit the loop if necessary
            // break;
        }
    }

    // Perform any necessary cleanup before exiting
}

void Gun::fire() {
    // Logic to send IR signal with currentPlayerID_
    ESP_LOGI("Gun", "Firing with player ID: %s", currentPlayerID_.c_str());
}

std::string Gun::getPlayerID() const {
    return currentPlayerID_;  // Return the current player ID
}
