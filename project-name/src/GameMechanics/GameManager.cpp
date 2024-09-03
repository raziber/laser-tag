#include "GameManager.h"
#include "BT.h"
#include "GameMode.h"
#include <Arduino.h>

namespace GameManager {
    Game myGame;

    void begin() {
        // Create the Game task pinned to core 1
        xTaskCreatePinnedToCore(
            gameTask,            // Task function
            "Game Task",         // Task name
            4096,                // Stack size (in words)
            NULL,                // Task input parameter
            1,                   // Priority of the task
            NULL,                // Task handle
            1                    // Core to which the task is pinned (1 in this case)
        );
    }

    void gameTask(void *parameter) {
        // Enter standby mode initially
        myGame.StandbyMode();

        // Main game loop
        while (true) {
            // Update the game state
            myGame.Update();

            // Handle inputs (e.g., from Bluetooth)
            InputType input = BT::ReceiveInput();  // Assuming a method like this exists
            myGame.HandleInput(input);

            // Handle game state transitions (e.g., starting a game)
            if (input == InputType::START_GAME) {
                GameMode selectedGameMode = BT::GetSelectedGameMode();  // Get the selected game mode
                myGame.Begin(selectedGameMode);  // Start the game with the selected mode
            }

            // Add a delay to yield to other tasks
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}
