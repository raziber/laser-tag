#pragma once

#include <Arduino.h>
#include <string>
#include <memory>
#include <cstdio>
#include <type_traits>

namespace Utils {
    extern SemaphoreHandle_t serialMutex;

    void safeSerialPrintln(const std::string& message);
    void safeSerialPrint(const std::string& message);
}
