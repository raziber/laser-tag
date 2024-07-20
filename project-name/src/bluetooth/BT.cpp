#include "BT.h"

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <memory>  // Include for smart pointers

std::unique_ptr<BluetoothSerial> SerialBT;
bool isConnected = false;
const char *deviceAddress = nullptr;

bool connectToDevice(const char *address) {
  if (!SerialBT->connect(address)) {
    Serial.println("Failed to connect. Make sure remote device is available and in range.");
    return false;
  } else {
    Serial.println("Connected successfully.");
    isConnected = true;
    deviceAddress = address;
    return true;
  }
}

void disconnectFromDevice() {
  if (isConnected) {
    SerialBT->disconnect();
    Serial.println("Disconnected successfully.");
    isConnected = false;
    deviceAddress = nullptr;
  } else {
    Serial.println("No device is currently connected.");
  }
}

void bluetoothTask(void *parameter) {
  while (true) {
    if (isConnected && SerialBT->available()) {
      String received = SerialBT->readString();
      Serial.println("Received: " + received);
      SerialBT->println("Echo: " + received);  // Echo back to sender
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);  // Yield to other tasks
  }
}

void bluetoothInit(){
    SerialBT = std::make_unique<BluetoothSerial>();  // Initialize the smart pointer
    SerialBT->begin("ESP32_BT");  // Bluetooth device name
    Serial.println("Bluetooth Started! Ready to pair.");

    // Create the Bluetooth task pinned to core 0
    xTaskCreatePinnedToCore(
        bluetoothTask,  // Task function
        "Bluetooth Task",  // Task name
        4096,  // Stack size (in words)
        NULL,  // Task input parameter
        1,  // Priority of the task
        NULL,  // Task handle
        0  // Core to which the task is pinned (0 in this case)
    );
}
