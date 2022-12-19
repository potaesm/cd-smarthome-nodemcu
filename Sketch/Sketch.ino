#define DEVMODE

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "LittleFS.h"

PulseOximeter POX;

#define NO_SENSOR 0
#define DS18B20_SENSOR 1
#define MAX30100_SENSOR 2
#define LIMIT_SENSOR 3
int sensor = NO_SENSOR;
byte reportNumber = 0;
byte sensorAttemptNumber = 0;
float prevoiusBPM = 0.0f;
float accTempC = 0.0f;
float accTempF = 0.0f;
float accBPM = 0.0f;
float accSpO2 = 0.0f;

unsigned long globalPreviousMillis = 0;

#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "Config.h"
#include "Stage.h"
#include "JsonHelper.h"
#include "WifiHelper.h"

WiFiClient wifiClient;
PubSubClient mqttClient(MQTT_BROKER, MQTT_PORT, wifiClient);

#include "OTAUpdateHelper.h"
#include "Utils.h"

void handleESP8266Update(String commit, String url)
{
  String response = "{}";
  // Send BIN_URL_RECEIVED
  response = addProperty(response, "id", DEVICE_ID);
  response = addProperty(response, "commit", commit);
  response = addProperty(response, "stage", BIN_URL_RECEIVED);
  sendMQTTMessage(response);
  // Disconnect MQTT
  mqttClient.unsubscribe(MQTT_TOPIC);
  mqttClient.disconnect();
  // Start update
  byte updateAttemptNumber = 0;
  while (updateAttemptNumber < 5)
  {
    connectWifi();
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
    // ESPhttpUpdate.closeConnectionsOnUpdate(false);
    ESPhttpUpdate.rebootOnUpdate(false);
    t_httpUpdate_return ESPHttpUpdateReturn = ESPhttpUpdate.update(wifiClient, url);
    removeData();
    // Reconnect MQTT and send result
    connectMQTTBroker(softwareReset);
    switch (ESPHttpUpdateReturn)
    {
    case HTTP_UPDATE_FAILED:
    {
      // Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      response = updateProperty(response, "stage", String(UPDATE_FAILED) + " (" + String(ESPhttpUpdate.getLastError()) + "): " + ESPhttpUpdate.getLastErrorString());
      updateAttemptNumber++;
      break;
    }
    case HTTP_UPDATE_NO_UPDATES:
    {
      // Serial.println("HTTP_UPDATE_NO_UPDATES");
      response = updateProperty(response, "stage", NO_UPDATES);
      updateAttemptNumber = 5;
      break;
    }
    case HTTP_UPDATE_OK:
    {
      // Serial.println("HTTP_UPDATE_OK");
      response = updateProperty(response, "stage", UPDATE_OK);
      updateAttemptNumber = 5;
      break;
    }
    }
  }
  sendMQTTMessage(response);
  softwareReset();
}

void mqttPayloadProcess(char *topic, byte *payload, unsigned int payloadLength)
{
  handleMQTTUpdateMessage(handleESP8266Update, topic, payload, payloadLength);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  connectWifi();
  // mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttPayloadProcess);
  connectMQTTBroker(softwareReset);
  sensor = beginSensor();
}

void loop()
{
  if (!isWiFiConnected())
  {
    connectWifi();
  }
  if (!mqttClient.connected())
  {
    connectMQTTBroker(softwareReset);
  }
  mqttClient.loop();

  // Main tasks
  switch (sensor)
  {
  case MAX30100_SENSOR:
  {
    POX.update();
    globalPreviousMillis = callbackRoutine(reportMAX30100, globalPreviousMillis, 1000);
    break;
  }
  case DS18B20_SENSOR:
  {
    globalPreviousMillis = callbackRoutine(reportDS18B20, globalPreviousMillis, 1000);
    break;
  }
  default:
    sensor = DS18B20_SENSOR;
    break;
  }
  if (reportNumber == MAX_REPORT_NUMBER)
  {
    sensor++;
    writeData(String(sensor));
    softwareReset();
  }
}