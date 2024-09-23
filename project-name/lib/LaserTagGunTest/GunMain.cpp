/**
 * @file    GunMain.cpp
 * @brief   Simple, single transmitter gun sketch.
 */

#include "LaserTagGun.hpp"

gpio_num_t IR_LED_GRPIO_PORT = GPIO_NUM_4;
IRProtocol IR_COMMUNICATION_PROTOCOL = IRProtocol::NEC;
LaserTagGun gun(IR_LED_GRPIO_PORT, IR_COMMUNICATION_PROTOCOL);

void setup() {
    static constexpr int SERIAL_BAUDRATE = 115200;
    Serial.begin(SERIAL_BAUDRATE);
    delay(1000);  // Wait for Serial to initialize

    esp_err_t err = gun.start();
    if (err != ESP_OK) {
        ESP_LOGE("GunMain", "Failed to start LaserTagGun: %s", esp_err_to_name(err));
    }
}

void loop() {
    // Main loop can be empty as FreeRTOS tasks handle the functionality
    vTaskDelay(pdMS_TO_TICKS(1000));
}
