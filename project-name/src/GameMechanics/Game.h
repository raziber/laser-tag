#pragma once

#include "GameMode.h"
#include "InputType.h"

class Game {
public:
    Game();
    void StandbyMode();  // Method to handle standby mode
    void Begin(GameMode mode);  // Starts the game with a selected mode
    void Update();  // Continuously updates the game state
    void HandleInput(InputType input);  // Handles user input
    void End();  // Ends the current game

private:
    GameMode currentMode;  // Stores the current game mode
    bool isActive;
};
