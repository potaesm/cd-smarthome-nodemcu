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

#define DEVMODE 1

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

void connectWifi(char *wifiSSID, char *wifiPassword)
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

void disconnectWifi()
{
    WiFi.mode(WIFI_OFF);
}

void checkInternet(WiFiClient wiFiClient, char *wifiSSID, char *wifiPassword, char *connectionTestHost, unsigned int connectionTestPort, char *connectionTestPath)
{
    bool networkCheck = false;
    if (!wiFiClient.connect(connectionTestHost, connectionTestPort))
    {
        return;
    }
    String requestHeaders = "GET " + String(connectionTestPath) + " HTTP/1.1\r\n" + "Host: " + String(connectionTestHost) + "\r\n" + "Connection: close\r\n\r\n";
    wiFiClient.print(requestHeaders);
    unsigned long apiTimeout = millis();
    while (!wiFiClient.available())
    {
        if (millis() - apiTimeout > 5000)
        {
            wiFiClient.stop();
            return;
        }
    }
    String respondLine;
    while (wiFiClient.connected())
    {
        respondLine = wiFiClient.readStringUntil('\n');
        if (respondLine == "\r")
        {
            networkCheck = true;
#if defined(DEVMODE)
            Serial.println("Internet is OK");
#endif
            break;
        }
    }
    if (!networkCheck)
    {
        WiFi.disconnect();
        delay(2500);
        __setHostname();
        WiFi.begin(wifiSSID, wifiPassword);
        __wifiConnectLoop(wifiSSID);
    }
}