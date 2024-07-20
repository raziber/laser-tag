#pragma once

void bluetoothTask(void *parameter);
void bluetoothInit();
void disconnectFromDevice();
bool connectToDevice(const char *address);