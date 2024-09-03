#include "Utils.h"
#include <Arduino.h>

namespace Utils {
    SemaphoreHandle_t serialMutex = xSemaphoreCreateMutex();

    void safeSerialPrintln(const std::string& message) {
        if (xSemaphoreTake(serialMutex, portMAX_DELAY)) {
            Serial.println(message.c_str());
            xSemaphoreGive(serialMutex);
        }
    }

    void safeSerialPrint(const std::string& message) {
        if (xSemaphoreTake(serialMutex, portMAX_DELAY)) {
            Serial.print(message.c_str());
            xSemaphoreGive(serialMutex);
        }
    }

    // Overloaded function for printing a single character
    void safeSerialPrintln(char character) {
        if (xSemaphoreTake(serialMutex, portMAX_DELAY)) {
            Serial.println(character);
            xSemaphoreGive(serialMutex);
        }
    }

    void safeSerialPrint(char character) {
        if (xSemaphoreTake(serialMutex, portMAX_DELAY)) {
            Serial.print(character);
            xSemaphoreGive(serialMutex);
        }
    }
}
