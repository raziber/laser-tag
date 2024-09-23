#include "LaserTagGun.hpp"

// Constructor
LaserTagGun::LaserTagGun(gpio_num_t buttonGpioNum, IRProtocol protocol)
    : buttonGpioNum_(buttonGpioNum), protocol_(protocol), currentCommand_(0x02), commandMutex_(nullptr) {
    commandMutex_ = xSemaphoreCreateMutex();
    if (commandMutex_ == nullptr) {
        ESP_LOGE("LaserTagGun", "Failed to create mutex");
    }
}

// Destructor
LaserTagGun::~LaserTagGun() {
    // Clean up resources
    if (commandMutex_ != nullptr) {
        vSemaphoreDelete(commandMutex_);
        commandMutex_ = nullptr;
    }
}

// Start function
esp_err_t LaserTagGun::start() {
    initTransmitter();
    initButton();
    initRFID();
    return ESP_OK;
}

void LaserTagGun::initTransmitter() {
    auto encoder = std::make_unique<IREncoder>(protocol_);
    int memBlockNum = 1;
    int clkDiv = 80;
    auto transmitter = std::make_unique<IRTransmitter>(std::vector<int>{buttonGpioNum_}, std::move(encoder), memBlockNum, clkDiv);
    transmitters_.push_back(std::move(transmitter));
}

void LaserTagGun::initButton() {
    // Configure GPIO for the button
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;  // No interrupts for polling method
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << buttonGpioNum_);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // Create the button task and pass 'this' as the parameter
    xTaskCreate(buttonTask, "ButtonTask", 2048, this, 5, &buttonTaskHandle_);
}

// Static button task function
void LaserTagGun::buttonTask(void* arg) {
    LaserTagGun* gun = static_cast<LaserTagGun*>(arg);
    if (gun == nullptr) {
        // Handle error
        vTaskDelete(NULL);
        return;
    }

    while (true) {
        int buttonState = gpio_get_level(gun->buttonGpioNum_);
        if (buttonState == 0) {  // Assuming active low
            gun->handleButtonPress();
            vTaskDelay(pdMS_TO_TICKS(200));  // Debounce delay
        } else {
            vTaskDelay(pdMS_TO_TICKS(10));   // Short delay
        }
    }
}

// Handle button press
esp_err_t LaserTagGun::handleButtonPress() {
    uint32_t address = 0x01;    // Example address
    uint32_t command = 0x02;    // Default command

    if (commandMutex_ == nullptr) {
        ESP_LOGE("LaserTagGun", "LaserTagGun mutex not initialized.");
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(commandMutex_, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE("LaserTagGun", "Failed to take mutex");
        return ESP_FAIL;
    }

    command = currentCommand_;
    xSemaphoreGive(commandMutex_);

    // Transmit to all transmitters
    for (auto& tx : transmitters_) {
        tx->transmitToAllPorts(address, command);
    }
}

void LaserTagGun::handleRFIDRead(const std::string& tagId) {
    currentTagId_ = tagId;
    // Derive command from tagId (e.g., hash, lookup table)
    currentCommand_ = deriveCommandFromTag(tagId);
    ESP_LOGI("RFID Tag Read: %s, Command set to: 0x%X\n", tagId.c_str(), currentCommand_);
}

uint32_t LaserTagGun::deriveCommandFromTag(const std::string& tagId) {
    // Implement logic to derive command
    // Example: Convert tagId to a number and use as command
    uint32_t command = std::hash<std::string>{}(tagId) & 0xFFFF;  // Use lower 16 bits
    return command;
}
