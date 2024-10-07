// Game.cpp
#include "Game.hpp"
#include "esp_log.h"

Game::Game()
    : isPaused_(false), isRunning_(true), gun_(std::make_unique<Gun>()) {}

void Game::startGame() {
    gun_->start();

    const uint32_t GAME_TASK_STACK_SIZE = 4096;
    const UBaseType_t GAME_TASK_PRIORITY = 5;
    try {
        // Create the game task to run the game loop
        gameTask_ = std::make_unique<Task>("GameTask", GAME_TASK_STACK_SIZE, GAME_TASK_PRIORITY, [this] { gameLoop(); });
    } catch (const std::runtime_error& e) {
        ESP_LOGE("Game", "Failed to create game task: %s", e.what());
    }
}

void Game::pauseGame() {
    isPaused_ = true;
    ESP_LOGI("Game", "Game paused.");
}

void Game::resumeGame() {
    isPaused_ = false;
    ESP_LOGI("Game", "Game resumed.");
}

void Game::stopGame() {
    isRunning_ = false;
    ESP_LOGI("Game", "Game stopped.");
    // Cleanup resources if necessary
}

void Game::gameLoop() {
    const uint32_t GAME_LOOP_DELAY_MS = 100;  // Configurable loop delay in milliseconds
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (isRunning_ && !gameTask_->shouldStop()) {
        if (!isPaused_) {
            // Game loop logic, e.g., checking game state, scoring, etc.
        }
        
        // Delay until the next iteration of the game loop
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(GAME_LOOP_DELAY_MS));
    }
}
