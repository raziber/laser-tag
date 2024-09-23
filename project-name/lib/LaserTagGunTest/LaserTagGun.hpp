#pragma once

/**
 * @file    LaserTagGun.hpp
 * @brief   Class definition for LaserTagGun.
 *
 * The `LaserTagGun` class encapsulates the functionality of a laser tag gun device.
 * It manages infrared (IR) transmissions, button inputs, and RFID interactions.
 * The class is designed for embedded systems using FreeRTOS, particularly on ESP32 platforms.
 *
 * ### Key Features:
 * - **IR Transmission:**
 *   - Manages one or more `IRTransmitter` instances to send IR signals.
 *   - Supports dynamic command values that can be updated based on RFID input.
 *   - Transmits commands when a button is pressed.
 * - **Button Handling:**
 *   - Monitors a physical button using GPIO input.
 *   - Uses a FreeRTOS task to handle button presses with debouncing.
 * - **RFID Integration:**
 *   - Reads RFID tags and updates the IR command based on the tag ID.
 *   - Allows dynamic changes to the gun's behavior (e.g., weapon type) via RFID.
 * - **Thread Safety:**
 *   - Utilizes mutexes to protect shared resources across tasks.
 * - **Configurable Parameters:**
 *   - Accepts GPIO numbers and IR protocol as constructor arguments for flexibility.
 *
 * ### Usage Example:
 * ```cpp
 * // Include necessary headers
 * #include "LaserTagGun.hpp"
 * 
 * // Create a LaserTagGun instance with specific button GPIO and protocol
 * LaserTagGun gun(GPIO_NUM_4, IRProtocol::NEC);
 * 
 * void setup() {
 *     Serial.begin(115200);
 *     delay(1000);  // Wait for Serial to initialize
 *     if (gun.start() != ESP_OK) {
 *         ESP_LOGE("Failed to start LaserTagGun");
 *         // Handle error
 *     }
 * }
 * 
 * void loop() {
 *     // Main loop can be empty as FreeRTOS tasks handle the functionality
 *     vTaskDelay(pdMS_TO_TICKS(1000));
 * }
 * ```
 *
 * ### Class Responsibilities:
 * - **Initialization:**
 *   - Initializes IR transmitters, button GPIO, and RFID reader.
 *   - Starts FreeRTOS tasks for button handling and RFID reading.
 * - **Event Handling:**
 *   - Handles button presses by transmitting IR commands.
 *   - Processes RFID reads to update the current command.
 * - **Resource Management:**
 *   - Ensures proper cleanup of tasks and resources in the destructor.
 *
 * ### Important Methods:
 * - `LaserTagGun(gpio_num_t buttonGpioNum, IRProtocol protocol)`: Constructor that accepts button GPIO number and IR protocol.
 * - `esp_err_t start()`: Initializes the system and starts tasks (if initialization is not in the constructor).
 * - `void handleButtonPress()`: Called when the button is pressed to transmit IR commands.
 * - `void handleRFIDRead(const std::string& tagId)`: Processes RFID tag reads to update the command.
 *
 * ### Notes:
 * - **FreeRTOS Dependency:** The class relies on FreeRTOS for task management and synchronization.
 * - **GPIO Configuration:** Ensure that the GPIO numbers provided are valid for your hardware and do not conflict with other peripherals.
 * - **Thread Safety:** Access to shared variables (e.g., `currentCommand_`) is protected using mutexes to ensure thread safety.
 * - **RFID Reader Integration:** The class assumes the existence of an `RFIDReader` class for RFID functionality. Implement this class according to your specific RFID hardware.
 *
 * ### Limitations:
 * - **No Error Checking on RFID Reads:** The example assumes successful RFID reads; implement error handling as needed.
 * - **Single Button Input:** The class currently handles only one button; extend as needed for additional inputs.
 *
 * ### Dependencies:
 * - **IRTransmitter.hpp:** Manages IR transmission.
 * - **IREncoder.hpp:** Encodes data into IR signal format.
 * - **FreeRTOS:** For task management and synchronization primitives.
 * - **ESP-IDF / Arduino Core for ESP32:** For GPIO and other hardware interactions.
 *
 * ### Example Extension:
 * To add additional transmitters (e.g., when attachments are connected), you can dynamically create `IRTransmitter` instances and add them to the `transmitters_` vector.
 *
 * @see IRTransmitter
 * @see IREncoder
 * @see IRProtocol
 * @see FreeRTOS
 * 
 * @author  Razi Berg
 * @date    23/09/2024
 *
 */

#include <vector>
#include <string>
#include <memory>
#include "IRTransmitter.hpp"
#include "IREncoder.hpp"

class LaserTagGun {
public:
    LaserTagGun(gpio_num_t buttonGpioNum, IRProtocol protocol);
    ~LaserTagGun();

    esp_err_t start();

private:
    void initTransmitter();
    void initButton();
    void initRFID();
    esp_err_t handleButtonPress();
    void handleRFIDRead(const std::string& tagId);
    uint32_t deriveCommandFromTag(const std::string& tagId);

    static void buttonTask(void* arg);
    static void rfidTask(void* arg);

    gpio_num_t buttonGpioNum_;
    IRProtocol protocol_;
    uint32_t currentCommand_;
    std::string currentTagId_;
    mutable SemaphoreHandle_t commandMutex_;

    std::vector<std::unique_ptr<IRTransmitter>> transmitters_;
    // Assume RFIDReader is a class you have for RFID
    RFIDReader rfidReader_;
    TaskHandle_t buttonTaskHandle_;
    TaskHandle_t rfidTaskHandle_;
};
