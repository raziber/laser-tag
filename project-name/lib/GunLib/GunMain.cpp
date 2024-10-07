// GunMain.cpp
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Game.hpp"
#include "esp_log.h"
#include <memory>

extern "C" void app_main() {
    try {
        // Create the Game instance and start the game
        std::unique_ptr<Game> game = std::make_unique<Game>();
        game->startGame();

        // Keep app_main running to maintain the scope of 'game'
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(1000)); // Delay to prevent watchdog timer reset
        }
    } catch (const std::runtime_error& e) {
        ESP_LOGE("Main", "Exception caught in setup: %s", e.what());
    }
}
