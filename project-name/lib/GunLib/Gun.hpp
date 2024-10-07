// Gun.hpp
#pragma once

#include "SPIBus.hpp"
#include "RFID.hpp"
#include "Button.hpp"
#include "Queue.hpp"
#include <optional>
#include <memory>
#include <string>
#include "freertos/FreeRTOS.h"
#include "Task.hpp"

class Gun {
public:
    Gun();
    void start();
    void fire();
    std::string getPlayerID() const;

    ~Gun();

private:
    std::unique_ptr<SPIBus> spiBus_;
    std::unique_ptr<RFID> rfid_;
    std::unique_ptr<Button> button_;
    std::string currentPlayerID_;
    std::unique_ptr<Queue<bool>> buttonQueue_;
    std::unique_ptr<Queue<std::string>> rfidQueue_;
    std::unique_ptr<Task> gunTask_;

    void gunTaskFunction();
};
