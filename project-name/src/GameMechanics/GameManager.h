#pragma once

#include "Game.h"

namespace GameManager {
    void begin();  // Initializes the game manager
    void gameTask(void *parameter);  // FreeRTOS task for game management
}
