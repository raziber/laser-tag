#pragma once

#include "SPIBus.hpp"
#include "RFID.hpp"
#include <optional>
#include "Arduino.h"

class Gun{
public:
    static std::optional<Gun> make();

    ~Gun();

private:
    SPIBus spiBus;
    RFID rfid;

    Gun();
};
