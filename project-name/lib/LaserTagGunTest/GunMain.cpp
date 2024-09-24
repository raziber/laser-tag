#include <Arduino.h>
#include "LaserTagGun.hpp"
#include "LaserTagGunConfig.hpp"
#include "SPIBusManager.hpp"
#include "SPIConfig.hpp"

using namespace LaserTagGunConfig;
using namespace SPIConfig;

// Declare pointers to the objects globally
std::unique_ptr<RFIDReader> rfidReader;
std::unique_ptr<LaserTagGun> laserTagGun;

void setup() {
    Serial.begin(115200);

    esp_err_t ret = SPIBusManager::initializeBus();
    if (ret != ESP_OK) {
        ESP_LOGE("Main", "Failed to initialize SPI bus");
        // Handle error accordingly
    }

    // Create MFRC522 instance
    auto mfrc522 = std::make_unique<MFRC522>(spiHost, ssPin, rstPin);

    // Create RFIDReader instance
    rfidReader = std::make_unique<RFIDReader>(std::move(mfrc522));

    // Create LaserTagGun instance
    laserTagGun = std::make_unique<LaserTagGun>(irLedPin, buttonPin, protocol, std::move(rfidReader));

    // Start the LaserTagGun
    ret = laserTagGun->start();
    if (ret != ESP_OK) {
        ESP_LOGE("Main", "Failed to start LaserTagGun");
        // Handle error accordingly
    }
}

void loop() {
    // Main loop can be empty as FreeRTOS tasks handle the functionality
    vTaskDelay(pdMS_TO_TICKS(1000));
}
