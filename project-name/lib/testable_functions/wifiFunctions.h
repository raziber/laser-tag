#ifdef EMBEDDED_BUILD
#ifndef WIFI_FUNCTIONS_H

void connectToWifi(const char* ssid, const char* password);
void wifiTask(void * parameter);

#endif // WIFI_FUNCTIONS_H
#endif // EMBEDDED_BUILD