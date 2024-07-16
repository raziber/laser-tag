#ifdef EMBEDDED_BUILD

#include "IRReceiver.h"
#include "IRTransmitter.h"
#include "wifiFunctions.h"

// Declare a semaphore handle
SemaphoreHandle_t wifiSemaphore;

// Create an array to hold IRReceiver instances
IRReceiver* irReceivers[NUM_SENSORS];

// Create an array to hold IRTransmitter instances
IRTransmitter* irTransmitters[NUM_SHOOTERS];

void setup() {
    Serial.begin(115200);

    // Create the semaphore before creating any tasks
    wifiSemaphore = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(wifiTask, "WiFiTask", 4096, NULL, 1, NULL, 0);

    // Delay to allow WiFi task to initialize
    vTaskDelay(pdMS_TO_TICKS(100));

    int channel = 0;

    // Initialize each IR receiver
    for (int i = 0; i < NUM_SENSORS; i++) {
        irReceivers[i] = new IRReceiver((gpio_num_t)sensorPins[i], (rmt_channel_t)channel++);
        irReceivers[i]->init();
    }

    // Initialize each IR transmitter
    for (int i = 0; i < NUM_SHOOTERS; i++) {
        irTransmitters[i] = new IRTransmitter((gpio_num_t)shooterPins[i], (rmt_channel_t)channel++);
        irTransmitters[i]->init();
    }

    if(channel > RMT_CHANNEL_MAX){
        Serial.printf("exceeded max RMT channels, used %d channels\n", channel);
    }

    Serial.println("All IR receivers and transmitters initialized.");
}

void loop() {
    // Wait for WiFi semaphore
    if (xSemaphoreTake(wifiSemaphore, portMAX_DELAY) == pdTRUE) {
        // Example usage: send a command from the first transmitter
        uint32_t address = 0xF8F8;
        uint32_t command = 0x0BF4;
        irTransmitters[0]->sendCommand(address, command);

        vTaskDelay(pdMS_TO_TICKS(3000)); // Sleep to let FreeRTOS manage tasks
    }
}

#else
int main(){
    
}
#endif
