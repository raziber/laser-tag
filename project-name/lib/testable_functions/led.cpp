#ifdef EMBEDDED_BUILD

#include "led.hpp"

void setLED(int state){
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, state);
}

#endif
