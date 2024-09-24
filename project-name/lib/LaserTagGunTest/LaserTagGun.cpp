#include "LaserTagGun.hpp"
#include "MFRC522.hpp"

using namespace MFRC522Constants;

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

void LaserTagGun::initRFID(){
    xTaskCreate(rfidTask, "RFID Task", 4096, NULL, 5, NULL);
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

// Static RFID task function
void LaserTagGun::buttonTask(void* arg){
    MFRC522 rfidReader;
    esp_err_t ret = rfidReader.initialize();
    if (ret != ESP_OK) {
        ESP_LOGE("RFID Task", "Failed to initialize MFRC522: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
    }

    while (true) {
        bool cardPresent = false;
        ret = rfidReader.isNewCardPresent(cardPresent);
        if (ret != ESP_OK) {
            ESP_LOGE("RFID Task", "Error checking for new card: %s", esp_err_to_name(ret));
        } else if (cardPresent) {
            std::array<uint8_t, MFRC522Constants::maxUidLength_> uid;
            uint8_t uidSize = 0;
            ret = rfidReader.readCardSerial(uid, uidSize);
            if (ret == ESP_OK) {
                ESP_LOGI("RFID Task", "Card UID:");
                for (uint8_t i = 0; i < uidSize; i++) {
                    printf("%02X ", uid[i]);
                }
                printf("\n");

                // Authenticate with Key A
                uint8_t keyA[MFRC522Constants::mifareKeySize_] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                ret = rfidReader.authenticate(4, static_cast<uint8_t>(PICCCommand::AuthenticateKeyA), keyA);
                if (ret == ESP_OK) {
                    // Read data from block 4
                    uint8_t buffer[18];
                    uint8_t bufferSize = sizeof(buffer);
                    ret = rfidReader.readBlock(4, buffer, &bufferSize);
                    if (ret == ESP_OK) {
                        ESP_LOGI("RFID Task", "Block 4 Data:");
                        for (int i = 0; i < 16; i++) {
                            printf("%02X ", buffer[i]);
                        }
                        printf("\n");
                    } else {
                        ESP_LOGE("RFID Task", "Failed to read block: %s", esp_err_to_name(ret));
                    }
                    rfidReader.stopCrypto();
                } else {
                    ESP_LOGE("RFID Task", "Authentication failed: %s", esp_err_to_name(ret));
                }
            } else {
                ESP_LOGE("RFID Task", "Failed to read card serial: %s", esp_err_to_name(ret));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Check every 500ms
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
