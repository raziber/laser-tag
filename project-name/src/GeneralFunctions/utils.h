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

    template<typename T>
    std::string to_string(T&& t) {
        if constexpr (std::is_same_v<std::decay_t<T>, String>) {
            return std::string(t.c_str());
        } else if constexpr (std::is_same_v<std::decay_t<T>, const char*>) {
            return std::string(t);
        } else if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
            return t;
        } else {
            return std::to_string(std::forward<T>(t));
        }
    }

    template<typename... Args>
    void safeSerialPrintln(Args&&... args) {
        std::string message = (to_string(std::forward<Args>(args)) + ...); // Fold expression to concatenate all arguments
        safeSerialPrintln(message);
    }

    template<typename... Args>
    void safeSerialPrint(Args&&... args) {
        std::string message = (to_string(std::forward<Args>(args)) + ...); // Fold expression to concatenate all arguments
        safeSerialPrint(message);
    }

    template<typename... Args>
    void safeSerialPrintf(const std::string& format, Args... args) {
        size_t size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
        if (size <= 0) {
            safeSerialPrintln("Error during formatting.");
            return;
        }
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args...);
        
        if (xSemaphoreTake(serialMutex, portMAX_DELAY)) {
            Serial.print(buf.get());
            Serial.print("\n");
            xSemaphoreGive(serialMutex);
        }
    }
}
