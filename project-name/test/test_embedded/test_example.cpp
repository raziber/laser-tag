#include <gtest/gtest.h>
#include <Arduino.h>
#include "led.hpp"

TEST(setLED, test_if_sets_led_to_high){
    setLED(HIGH);
    pinMode(LED_BUILTIN, INPUT);
    ASSERT_EQ(digitalRead(LED_BUILTIN), HIGH);
}

TEST(setLED, test_if_sets_led_to_low){
    setLED(LOW);
    pinMode(LED_BUILTIN, INPUT);
    ASSERT_EQ(digitalRead(LED_BUILTIN), LOW);
}
