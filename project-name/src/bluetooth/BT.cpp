#include "BT.h"

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <memory>  // Include for smart pointers
#include "utils.h"

namespace BT {
	// Use a unique pointer for BluetoothSerial
	std::unique_ptr<BluetoothSerial> SerialBT = nullptr;
	bool isConnected = false;
	std::string deviceAddress;

	bool connectToDevice(const std::string& address) {
		if (!SerialBT->connect(address.c_str())) {
			Utils::safeSerialPrintf("Failed to connect to %s. Make sure remote device is available and in range.", address.c_str());
			return false;
		} else {
			Utils::safeSerialPrintf("Connected to %s successfully.", address.c_str());
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

	void bluetoothTask(void *parameter) {
		while (true) {
			if (isConnected && SerialBT->available()) {
				String received = SerialBT->readString();
				Utils::safeSerialPrintf("Received: %s", received.c_str());
				SerialBT->println("Echo: " + received);  // Echo back to sender
			}
			vTaskDelay(10 / portTICK_PERIOD_MS);  // Yield to other tasks
		}
	}

	void bluetoothInit() {
		SerialBT = std::make_unique<BluetoothSerial>();  // Initialize the smart pointer
		SerialBT->begin("ESP32_BT");  // Bluetooth device name
		Utils::safeSerialPrintln("Bluetooth Started! Ready to pair.");

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
}