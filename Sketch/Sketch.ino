#define DEVMODE

#include <OneWire.h>
#include <DallasTemperature.h>

#define NO_SENSOR 0
#define TEMPERATURE_SENSOR 1

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

byte sensor = NO_SENSOR;
const long interval = 1000;
unsigned long previousMillis = 0;

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
    sendMQTTMessage(response);
    break;
  }
  case HTTP_UPDATE_NO_UPDATES:
  {
    // Serial.println("HTTP_UPDATE_NO_UPDATES");
    response = updateProperty(response, "stage", NO_UPDATES);
    sendMQTTMessage(response);
    break;
  }
  case HTTP_UPDATE_OK:
  {
    // Serial.println("HTTP_UPDATE_OK");
    response = updateProperty(response, "stage", UPDATE_OK);
    sendMQTTMessage(response);
    break;
  }
  }
  softwareReset();
}

void mqttPayloadProcess(char *topic, byte *payload, unsigned int payloadLength)
{
  handleMQTTUpdateMessage(handleESP8266Update, topic, payload, payloadLength);
}

void softwareReset()
{
  delay(100);
  ESP.restart();
}

void setup()
{
  Serial.begin(115200);
  connectWifi();
  // mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttPayloadProcess);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  if (!mqttClient.connected())
  {
    connectMQTTBroker(softwareReset);
  }
  mqttClient.loop();

  // Main tasks
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
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
    default:
      sensor = NO_SENSOR;
      break;
    }
  }
}
