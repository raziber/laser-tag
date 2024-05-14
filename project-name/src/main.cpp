#ifdef EMBEDDED_BUILD

#include <Arduino.h>
#include "configuration.h"
#include "taskManager.h"

void setup() {
    Serial.begin(115200);
    
    createSensorTasks();
}

void loop() {
}

#else
int main(){
    
}
#endif
