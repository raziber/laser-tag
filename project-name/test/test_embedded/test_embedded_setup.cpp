#include <gtest/gtest.h>
#include <Arduino.h>
#include "irInitFunctions.h"
#include "BT.h"
#include "ProtocolTypes.h"

void setup(){
    static constexpr int BAUD_RATE = 115200;
    Serial.begin(BAUD_RATE);
    BT::begin();
    IRDevices::begin(Protocol::SAMSUNG);

    testing::InitGoogleTest();
    if(!RUN_ALL_TESTS());
}

void loop(){

}
