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

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include "Config.h"
#include "Stage.h"
#include "JsonHelper.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

bool blinkStatus = false;
const long interval = 1000;
unsigned long previousMillis = 0;

void handleMQTTMessage(void (*)(String commit, String url));

void handleMQTTMessage(void (*function)(String commit, String url)){
  if (mqttClient.parseMessage() && String(mqttClient.messageTopic()) == TOPIC) {
    String message = "";
    while (mqttClient.available()) {
      char currentChar = (char)mqttClient.read();
      message.concat(currentChar);
    }
    message = parseJson(message);
    Serial.println("Message: " + message);
    String id = getValue(message, "id");
    String commit = getValue(message, "commit");
    String url = getValue(message, "url");
    bool isAllowed = (id == "") ? true : (id == DEVICE_ID) ? true : false;
    if (url != "" && isAllowed) {
      (*function)(commit, url);
    }
  }
}

void handleESP8266Update(String commit, String url) {
  String response = "{}";
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
  connectMQTTBroker();
  switch (ESPHttpUpdateReturn) {
    case HTTP_UPDATE_FAILED: {
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        response = modifyValue(response, "stage", String(UPDATE_FAILED) + " (" + String(ESPhttpUpdate.getLastError()) + "): " + ESPhttpUpdate.getLastErrorString());
        sendMQTTMessage(response);
        break;
      }
    case HTTP_UPDATE_NO_UPDATES: {
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        response = modifyValue(response, "stage", NO_UPDATES);
        sendMQTTMessage(response);
        break;
      }
    case HTTP_UPDATE_OK: {
        Serial.println("HTTP_UPDATE_OK");
        response = modifyValue(response, "stage", UPDATE_OK);
        sendMQTTMessage(response);
        break;
      }
  }
  delay(100);
  ESP.restart();
}

void sendMQTTMessage(String message) {
  mqttClient.beginMessage(TOPIC, message.length(), MQTT_MESSAGE_RETAINED, MQTT_MESSAGE_QOS);
  mqttClient.print(message);
  mqttClient.endMessage();
}

void connectMQTTBroker() {
  if (!mqttClient.connect(MQTT_BROKER, MQTT_PORT)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    delay(100);
    ESP.restart();
  }
  Serial.println("Connected to broker " + String(MQTT_BROKER));
  mqttClient.subscribe(TOPIC, MQTT_SUBSCRIBE_QOS);
  Serial.println("Subscribed to topic " + String(TOPIC));
}

void setup() {
  Serial.begin(115200);

  connectWifi(WIFI_SSID, WIFI_PASS);
  checkInternet(wifiClient, WIFI_SSID, WIFI_PASS);

  mqttClient.setId(DEVICE_ID);
  mqttClient.setUsernamePassword(MQTT_USERNAME, MQTT_PASS);

  connectMQTTBroker();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  while (!mqttClient.connected()) {
    connectMQTTBroker();
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
