#pragma once

#include "Arduino.h"

class Button{
public:
    static std::optional<Button> make(gpio_num_t gpio);

    ~Button() = default;
private:
    gpio_num_t gpio_;
    bool buttonHandle_;

    Button(gpio_num_t gpio);

    static gpio_config_t configureGpio(gpio_num_t gpio);
    bool monitor();
};
