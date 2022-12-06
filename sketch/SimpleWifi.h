#ifndef SimpleWifi_h
#define SimpleWifi_h
#include "Arduino.h"
#if defined(ESP8266)
#include "ESP8266WiFi.h"
#elif defined(ESP32)
#include "WiFi.h"
#endif

extern void connectWifi(char *wifiSSID, char *wifiPassword);
extern void disconnectWifi();
extern bool isWiFiConnected();
extern void checkInternet(WiFiClient wiFiClient, char *wifiSSID, char *wifiPassword, char *connectionTestHost = "172.217.8.96", unsigned int connectionTestPort = 80, char *connectionTestPath = "/");

#endif