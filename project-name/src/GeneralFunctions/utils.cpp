#include "Utils.h"

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
}
