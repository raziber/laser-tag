#include <Arduino.h>
#include <string>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void bluetoothLoop();

void setup() {
    Serial.begin(115200);
    SerialBT.begin("ESP32test"); //Bluetooth device name
    Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
    bluetoothLoop();
}

void bluetoothLoop() {
    if (Serial.available()) {
        SerialBT.write(Serial.read());
    }
    if (SerialBT.available()) {
        Serial.write(SerialBT.read());
    }
    delay(20);
}
