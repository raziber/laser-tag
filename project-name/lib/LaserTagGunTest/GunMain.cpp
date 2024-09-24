#include <Arduino.h>
#include "LaserTagGun.hpp"
#include "LaserTagGunConstants.hpp"

using namespace LaserTagGunConstants;

LaserTagGun laserTagGun(irLedPin, buttonPin, protocol);

void setup() {
    Serial.begin(115200);

    if (laserTagGun.start() != ESP_OK) {
        ESP_LOGE("Main", "Failed to start LaserTagGun");
        // Handle error accordingly
    }
}

void loop() {
    // Main loop can be empty as FreeRTOS tasks handle the functionality
    vTaskDelay(pdMS_TO_TICKS(1000));
}
