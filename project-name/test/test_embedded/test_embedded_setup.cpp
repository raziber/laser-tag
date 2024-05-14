#include <gtest/gtest.h>
#include <Arduino.h>

void setup(){
    Serial.begin(115200);
    delay(1000);

    testing::InitGoogleTest();
    if(!RUN_ALL_TESTS());
}

void loop(){

}
