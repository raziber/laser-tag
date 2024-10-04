#include <Arduino.h>
#include "LaserTagGun.hpp"
#include "LaserTagGunConfig.hpp"
#include "SPIBus.hpp"
#include "SPIConfig.hpp"
#include "Macros.hpp"
#include "ErrorStates.hpp"

// using namespace LaserTagGunConfig;
// using namespace SPIConfig;

// template <typename T>
// using UPtr = std::unique_ptr<T>;

// // Declare pointers to the objects globally
// //std::unique_ptr<RFIDReader> rfidReader;
// UPtr<RFIDReader> rfidReader;
// std::unique_ptr<LaserTagGun> laserTagGun;





// CHECK(do_something)
// MAKE_AND_CHECK()

// CHECK(value, "message")

void setup() {
    Serial.begin(115200);

    auto spiBus = MAKE_WITH_ARGS(SPIBus, SPIConfig::spiHost);

    // Create MFRC522 instance
    auto mfrc522 = std::make_unique<MFRC522>(SPIConfig::spiHost, spiBus.value(), SPIConfig::ssPin, SPIConfig::rstPin);

    // Create RFIDReader instance
    rfidReader = std::make_unique<RFIDReader>(mfrc522);

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
