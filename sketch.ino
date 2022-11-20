#include "SimpleWifi.h"
#include "SimpleOta.h"
#include <ESP8266HTTPClient.h>
#include "JsonMapper.h"

#define INTERVAL 1000
unsigned long previousMillis = 0;
Json greeting;

char *wifiSSID = "GuitarZeed_WiFi";
char *wifiPassword = "guitarzeed999";
HTTPClient http;
WiFiClient wiFiClient;

// MQTT
#include <PubSubClient.h>
PubSubClient client(wiFiClient);
String topic = "main/update";
const char* mqttServer = "woodpecker.rmq.cloudamqp.com";
int mqttPort = 1883;
const char* mqttUsername = "dahrusvc:dahrusvc";
const char* mqttPassword = "6lYM_XMYbBTM-rfU3vg4Qsdfmx8J1TlA";
const char* mqttClientId = "ESP8266Client";

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqttClientId, mqttUsername, mqttPassword)) {
      Serial.println("Connected");
      client.publish(topic.c_str(), greeting.getJson().c_str());
      client.subscribe(topic.c_str());
    } else {
      Serial.print("Failed, rc = ");
      Serial.print(client.state());
      Serial.println(", try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttPayloadProcess(char* mqttTopic, byte* mqttPayload, unsigned int mqttPayloadLength) {
  Json json;
  json.setJson(String((char*) mqttPayload));
  String jsonString = json.getJson();
  Serial.print("Parsed json: ");
  Serial.println(jsonString);
  String url = getJsonStringProperty(jsonString, "url");
  if (getJsonBooleanProperty(jsonString, "update") && url.length() > 0) {
    initializeOta(wiFiClient, wifiSSID, wifiPassword, url);
  }
}

void setup() {
  Serial.begin(115200);
  greeting.addStringProperty("message", "V0");
  connectWifi(wifiSSID, wifiPassword);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttPayloadProcess);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > INTERVAL) {
    previousMillis = currentMillis;
    Serial.println(greeting.getJson());
  }
}