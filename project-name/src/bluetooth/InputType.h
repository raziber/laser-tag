#pragma once

enum class InputType {
    NONE,               // Default, no action
    SELECT_GAME_MODE,   // Command to select a game mode
    START_GAME,         // Command to start the game
    END_GAME,           // Command to end the game
    PAUSE_GAME,         // Command to pause the game
    RESUME_GAME,        // Command to resume the game
    // Add more commands as needed
};
