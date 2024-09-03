#include "Game.h"
#include "BT.h"
#include "Utils.h"
#include <Arduino.h>

Game::Game() : currentMode(GameMode::NONE), isActive(false) {}

void Game::StandbyMode() {
    while (true) {
        char receivedData;

        // Wait for data from the Bluetooth queue
        if (xQueueReceive(BT::btQueue, &receivedData, portMAX_DELAY) == pdPASS) {
            Utils::safeSerialPrint("Received in StandbyMode: ");
            Utils::safeSerialPrintln(receivedData);

            // Process the received data using the BT::ProcessReceivedData function
            InputType input = BT::ProcessReceivedData(receivedData);

            if (input == InputType::SELECT_GAME_MODE) {
                currentMode = BT::GetSelectedGameMode();  // Select game mode via Bluetooth
                Begin(currentMode);  // Start the game immediately after mode selection
                break;
            }
        }

        // Add a small delay to avoid busy waiting
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void Game::Begin(GameMode mode) {
    isActive = true;
    currentMode = mode;

    // Initialize mode-specific settings here
    Utils::safeSerialPrint("Game mode started: ");
    Utils::safeSerialPrintln(static_cast<int>(mode));
    // For example, set up game logic based on the selected mode
}

void Game::Update() {
    if (isActive) {
        // Update game logic based on the current mode
        switch (currentMode) {
            case GameMode::GUN_BATTLE:
                // Update Gun Battle logic
                break;
            case GameMode::FFA:
                // Update FFA logic
                break;
            case GameMode::TEAMS:
                // Update Teams logic
                break;
            case GameMode::CAPTURE_THE_FLAG:
                // Update Capture the Flag logic
                break;
            default:
                break;
        }

        // Handle game state transitions or check for end conditions
        // Optionally handle other inputs if necessary
    }
}

void Game::HandleInput(InputType input) {
    // Handle various inputs (e.g., end game, pause, etc.)
    if (input == InputType::END_GAME && isActive) {
        End();
    }
}

void Game::End() {
    isActive = false;
    currentMode = GameMode::NONE;
    Utils::safeSerialPrintln("Game ended.");
    // Perform any necessary cleanup
}
