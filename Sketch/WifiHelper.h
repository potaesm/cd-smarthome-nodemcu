#ifndef WifiHelper_h
#define WifiHelper_h
#include "Arduino.h"
#if defined(ESP8266)
#warning "Using ESP8266"
#include "ESP8266WiFi.h"
#elif defined(ESP32)
#warning "Using ESP32"
#include "WiFi.h"
#else
#error "No ESP8266 or ESP32 detected"
#endif

#include "Config.h"

bool isWiFiConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

void __setHostname()
{
    randomSeed(analogRead(0));
    WiFi.hostname("ESP-Host-" + String(random(1024)));
}

void __wifiConnectLoop(char *wifiSSID)
{
    byte wifiConnectCount = 0;
#if defined(DEVMODE)
    Serial.println("Connecting");
#endif
    while (!isWiFiConnected())
    {
#if defined(DEVMODE)
        Serial.print(".");
#endif
        delay(1000);
        wifiConnectCount++;
        if (wifiConnectCount > 20)
        {
            ESP.reset();
        }
    }
#if defined(DEVMODE)
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(wifiSSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
#endif
}

void connectWifi(char *wifiSSID = WIFI_SSID, char *wifiPassword = WIFI_PASS)
{
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    __setHostname();
    WiFi.begin(wifiSSID, wifiPassword);
    WiFi.persistent(true);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        __wifiConnectLoop(wifiSSID);
    }
}

#endif