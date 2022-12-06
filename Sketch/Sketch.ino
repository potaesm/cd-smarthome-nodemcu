#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

#include <ArduinoMqttClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

#define DEVMODE

#include "Config.h"
#include "Stage.h"
#include "JsonHelper.h"
#include "WifiHelper.h"
#include "OTAUpdateHelper.h"

bool blinkStatus = false;
const long interval = 1000;
unsigned long previousMillis = 0;

void handleESP8266Update(String commit, String url) {
  String response = "{}";
  // Send BIN_URL_RECEIVED
  response = addProperty(response, "id", DEVICE_ID);
  response = addProperty(response, "commit", commit);
  response = addProperty(response, "stage", BIN_URL_RECEIVED);
  sendMQTTMessage(response);
  mqttClient.stop();
  // Start update
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
  // ESPhttpUpdate.closeConnectionsOnUpdate(false);
  ESPhttpUpdate.rebootOnUpdate(false);
  t_httpUpdate_return ESPHttpUpdateReturn = ESPhttpUpdate.update(wifiClient, url);
  // Send result
  connectMQTTBroker(softwareReset);
  switch (ESPHttpUpdateReturn) {
    case HTTP_UPDATE_FAILED: {
        // Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        response = updateProperty(response, "stage", String(UPDATE_FAILED) + " (" + String(ESPhttpUpdate.getLastError()) + "): " + ESPhttpUpdate.getLastErrorString());
        sendMQTTMessage(response);
        break;
      }
    case HTTP_UPDATE_NO_UPDATES: {
        // Serial.println("HTTP_UPDATE_NO_UPDATES");
        response = updateProperty(response, "stage", NO_UPDATES);
        sendMQTTMessage(response);
        break;
      }
    case HTTP_UPDATE_OK: {
        // Serial.println("HTTP_UPDATE_OK");
        response = updateProperty(response, "stage", UPDATE_OK);
        sendMQTTMessage(response);
        break;
      }
  }
  softwareReset();
}

void softwareReset() {
  delay(100);
  ESP.restart();
}

void setup() {
  Serial.begin(115200);

  connectWifi();

  mqttClient.setId(DEVICE_ID);
  mqttClient.setUsernamePassword(MQTT_USERNAME, MQTT_PASS);
  connectMQTTBroker(softwareReset);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTTBroker(softwareReset);
  }
  handleMQTTMessage(handleESP8266Update);

  // do something else
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    blinkStatus = !blinkStatus;
    digitalWrite(LED_BUILTIN, blinkStatus);
  }
}
