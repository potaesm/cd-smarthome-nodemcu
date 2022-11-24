#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

#include "SimpleWifi.h"
#include "JsonMapper.h"

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#define DEVICE_ID "DEVICE_ID"

#define BIN_URL_RECEIVED "BIN_URL_RECEIVED"
#define UPDATE_FAILED "UPDATE_FAILED"
#define NO_UPDATES "NO_UPDATES"
#define UPDATE_OK "UPDATE_OK"

#include "arduino_secrets.h"
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
char *wifiSSID = SECRET_SSID;
char *wifiPassword = SECRET_PASS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "puffin.rmq2.cloudamqp.com";
int port = 1883;
bool retained = false;
int qos = 2;
bool dup = false;
const char topic[]  = "main/update";
bool blinkStatus = false;
const long interval = 1000;

unsigned long previousMillis = 0;

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void setup() {
  Serial.begin(115200);

  connectWifi(wifiSSID, wifiPassword);
  checkInternet(wifiClient, wifiSSID, wifiPassword);

  mqttClient.setId(DEVICE_ID);
  mqttClient.setUsernamePassword(MQTT_USERNAME, MQTT_PASS);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }
  Serial.println("MQTT connected to broker " + String(broker));
  mqttClient.subscribe(topic, qos);
  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);
  Serial.println("MQTT subscribed to topic " + String(topic));
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // check for incoming messages
  int messageSize = mqttClient.parseMessage();
  if (messageSize && String(mqttClient.messageTopic()) == String(topic)) {
    String message = "";
    while (mqttClient.available()) {
      char currentChar = (char)mqttClient.read();
      message.concat(currentChar);
    }
    Json jsonMessage;
    jsonMessage.setJson(message);
    String jsonMessageString = jsonMessage.getJson();
    Serial.println("Parsed message: " + jsonMessageString);
    String url = getJsonStringProperty(jsonMessageString, "url");
    String id = getJsonStringProperty(jsonMessageString, "id");
    bool isAllowed = (id == "") ? true : (id == DEVICE_ID) ? true : false;
    if (url != "" && isAllowed) {
      Json response;
      response.addStringProperty("id", DEVICE_ID);
      response.cloneProperty("commit", jsonMessageString);
      response.patchStringProperty("stage", BIN_URL_RECEIVED);
      mqttClient.beginMessage(topic, response.getJson().length(), retained, qos, dup);
      mqttClient.print(response.getJson());
      mqttClient.endMessage();
      // Start update
      ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
      ESPhttpUpdate.rebootOnUpdate(false);
      ESPhttpUpdate.onStart(update_started);
      ESPhttpUpdate.onEnd(update_finished);
      ESPhttpUpdate.onProgress(update_progress);
      ESPhttpUpdate.onError(update_error);
      t_httpUpdate_return ret = ESPhttpUpdate.update(wifiClient, url);
      if (!mqttClient.connect(broker, port)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());
        while (1);
      }
      Serial.println("MQTT connected to broker " + String(broker));
      switch (ret) {
        case HTTP_UPDATE_FAILED: {
            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            response.patchStringProperty("stage", String(UPDATE_FAILED) + " (" + String(ESPhttpUpdate.getLastError()) + "): " + ESPhttpUpdate.getLastErrorString());
            mqttClient.beginMessage(topic, response.getJson().length(), retained, qos, dup);
            mqttClient.print(response.getJson());
            mqttClient.endMessage();
            break;
          }
        case HTTP_UPDATE_NO_UPDATES: {
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            response.patchStringProperty("stage", NO_UPDATES);
            mqttClient.beginMessage(topic, response.getJson().length(), retained, qos, dup);
            mqttClient.print(response.getJson());
            mqttClient.endMessage();
            break;
          }
        case HTTP_UPDATE_OK: {
            Serial.println("HTTP_UPDATE_OK");
            response.patchStringProperty("stage", UPDATE_OK);
            mqttClient.beginMessage(topic, response.getJson().length(), retained, qos, dup);
            mqttClient.print(response.getJson());
            mqttClient.endMessage();
            delay(100);
            ESP.restart();
            break;
          }
      }
    }
  }

  // do something else
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    blinkStatus = !blinkStatus;
    digitalWrite(LED_BUILTIN, blinkStatus);
  }
}