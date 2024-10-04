#include "Gun.hpp"

#include "Macros.hpp"
#include "SPIConfig.hpp"
#include "RFIDConfig.hpp"
#include "ErrorStates.hpp"

std::optional<Gun> Gun::make(){
    gpio_num_t BUTTON_GPIO = GPIO_NUM_30;
    TickType_t BUTTON_DEBOUNCE_DURATION = pdMS_TO_TICKS(50);

    Button button = MAKE_WITH_ARGS(Button, BUTTON_GPIO, BUTTON_DEBOUNCE_DURATION);
    SPIBus spiBus = MAKE_WITH_ARGS(SPIBus, SPIConfig::spiHost);
    RFID rfid = MAKE_WITH_ARGS(RFID, spiBus, RFIDConfig::csPin);

    auto pTask = MAKE_WITH_ARGS(Task);  // TODO: improve naming

    return std::make_optional<Gun>(spiBus, rfid, button, pTask);
}

Gun::Gun(SPIBus spiBus, RFID rfid, Button button, std::shared_ptr<Task> pTask) : spiBus_(spiBus), rfid_(rfid), button_(button), pTask_(pTask){}
