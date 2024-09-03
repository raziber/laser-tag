#include "BT.h"
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <memory>  // Include for smart pointers
#include "utils.h"
#include "InputType.h"
#include "GameMode.h"

namespace BT {
    std::unique_ptr<BluetoothSerial> SerialBT = nullptr;
    bool isConnected = false;
    std::string deviceAddress;

    QueueHandle_t btQueue = nullptr;

    bool connectToDevice(const std::string& address) {
        if (!SerialBT->connect(address.c_str())) {
            Serial.printf("Failed to connect to %s. Make sure remote device is available and in range.", address.c_str());
            return false;
        } else {
            Serial.printf("Connected to %s successfully.", address.c_str());
            isConnected = true;
            deviceAddress = address;
            return true;
        }
    }

    void disconnectFromDevice() {
        if (isConnected) {
            SerialBT->disconnect();
            Utils::safeSerialPrintln("Disconnected successfully.");
            isConnected = false;
            deviceAddress.clear();
        } else {
            Utils::safeSerialPrintln("No device is currently connected.");
        }
    }

    InputType ReceiveInput() {
        char receivedData;

        // Wait for data from the Bluetooth queue
        if (xQueueReceive(btQueue, &receivedData, portMAX_DELAY) == pdPASS) {
            // Process the received data using the BT::ProcessReceivedData function
            return ProcessReceivedData(receivedData);
        }

        // If no data was received, return NONE
        return InputType::NONE;
    }

    InputType ProcessReceivedData(char receivedData) {
        switch (receivedData) {
            case 'S':
                return InputType::SELECT_GAME_MODE;
            case 'E':
                return InputType::END_GAME;
            case 'P':
                return InputType::PAUSE_GAME;
            case 'R':
                return InputType::RESUME_GAME;
            case 'B':
                return InputType::START_GAME;
            default:
                return InputType::NONE;
        }
    }

    // Implementation of GetSelectedGameMode
    GameMode GetSelectedGameMode() {
        char receivedData;

        // Wait for the next character that indicates the game mode
        if (xQueueReceive(btQueue, &receivedData, portMAX_DELAY) == pdPASS) {
            switch (receivedData) {
                case '1':
                    return GameMode::GUN_BATTLE;
                case '2':
                    return GameMode::FFA;
                case '3':
                    return GameMode::TEAMS;
                case '4':
                    return GameMode::CAPTURE_THE_FLAG;
                // Add more cases for additional game modes
                default:
                    return GameMode::NONE;
            }
        }

        // Return NONE if no valid game mode was selected
        return GameMode::NONE;
    }

    void bluetoothTask(void *parameter) {
        if (btQueue == nullptr) {
            btQueue = xQueueCreate(10, sizeof(char));
        }

        while (true) {
            if (SerialBT->available()) {
                char received = SerialBT->read();
                Serial.printf("%c", received);

                if (xQueueSend(btQueue, &received, portMAX_DELAY) != pdPASS) {
                    Serial.println("Failed to send data to the queue.");
                }
            }
            if (Serial.available()) {
                char received = Serial.read();
                SerialBT->write(received);
            }
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }

    void begin() {
        SerialBT = std::make_unique<BluetoothSerial>();
        SerialBT->begin("ESP32_BT");
        Utils::safeSerialPrintln("Bluetooth Started! Ready to pair.");

        xTaskCreatePinnedToCore(
            bluetoothTask,
            "Bluetooth Task",
            4096,
            NULL,
            1,
            NULL,
            0
        );
    }
}
