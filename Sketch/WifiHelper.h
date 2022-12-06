#ifndef WifiHelper_h
#define WifiHelper_h
#include "Arduino.h"
#if defined(ESP8266)
#include "ESP8266WiFi.h"
#elif defined(ESP32)
#include "WiFi.h"
#endif

extern void connectWifi(char *wifiSSID, char *wifiPassword);
extern bool isWiFiConnected();

#endif