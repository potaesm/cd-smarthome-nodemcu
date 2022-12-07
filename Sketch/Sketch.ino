#define DEVMODE

#include <OneWire.h>
#include <DallasTemperature.h>

#define NO_SENSOR 0
#define TEMPERATURE_SENSOR 1
#define HEART_RATE_SENSOR 2
byte sensor = NO_SENSOR;
bool enableHeartRateSensor = false;
byte heartRateReadIntervalCounter = 0;
unsigned long heartRatePreviousMillis = 0;

#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "Config.h"
#include "Stage.h"
#include "JsonHelper.h"
#include "WifiHelper.h"

WiFiClient wifiClient;
PubSubClient mqttClient(MQTT_BROKER, MQTT_PORT, wifiClient);

unsigned long globalPreviousMillis = 0;

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

void handleHeartRateSensor()
{
  if (enableHeartRateSensor)
  {
    Serial.println(analogRead(0));
  }
}

void handleSensors()
{
  sensor++;
  switch (sensor)
  {
  case TEMPERATURE_SENSOR:
  {
    // GPIO where the DS18B20 is connected to
    OneWire oneWire(D3);
    DallasTemperature DS18B20(&oneWire);
    DS18B20.begin();
    DS18B20.requestTemperatures();
    float temperatureC = DS18B20.getTempCByIndex(0);
    // float temperatureF = DS18B20.getTempFByIndex(0);
    Serial.print(temperatureC);
    Serial.println("ºC");
    // Serial.print(temperatureF);
    // Serial.println("ºF");
    break;
  }
  case HEART_RATE_SENSOR:
  {
    // Read for 1000 * 10 = 10 seconds
    enableHeartRateSensor = heartRateReadIntervalCounter <= 10;
    if (enableHeartRateSensor)
    {
      heartRateReadIntervalCounter++;
      sensor--;
    }
    else
    {
      heartRateReadIntervalCounter = 0;
    }
    break;
  }
  default:
    sensor = NO_SENSOR;
    break;
  }
}

void setup()
{
  Serial.begin(9600);
  connectWifi();
  // mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttPayloadProcess);
  // pinMode(0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
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
  globalPreviousMillis = callbackRoutine(handleSensors, globalPreviousMillis, 1000);
  heartRatePreviousMillis = callbackRoutine(handleHeartRateSensor, heartRatePreviousMillis, 10);
}
