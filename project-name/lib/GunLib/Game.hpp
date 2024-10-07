#pragma once

#include <memory>
#include "Gun.hpp"

class Game {
public:
    Game();
    void startGame();
    void pauseGame();
    void resumeGame();
    void stopGame();

    ~Game() = default;

private:
    std::unique_ptr<Gun> gun_;
    bool isPaused_;
    bool isRunning_;
    std::unique_ptr<Task> gameTask_;
    void gameLoop();
};
