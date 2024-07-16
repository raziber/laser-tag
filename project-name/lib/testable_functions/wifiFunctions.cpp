#ifdef EMBEDDED_BUILD

#include "wifiFunctions.h"
#include <WiFi.h>

extern SemaphoreHandle_t wifiSemaphore;

const char* wifi_ssid = "Shiranga";
const char* wifi_password = "minky123";

void connectToWifi(const char* ssid, const char* password){
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while(WiFi.status() != WL_CONNECTED){
        delay(200);
        Serial.print(".");
    }
    Serial.println("\nConnected successfully");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void wifiTask(void * parameter) {
    connectToWifi(wifi_ssid, wifi_password);
    // Give the semaphore to signal that WiFi is connected
    xSemaphoreGive(wifiSemaphore);
    vTaskDelete(NULL);  // End task if no longer needed
}

#endif // EMBEDDED_BUILD