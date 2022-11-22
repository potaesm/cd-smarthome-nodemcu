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

#define BIN_URL_RECEIVED "BIN_URL_RECEIVED"
#define BIN_DOWNLOADING "BIN_DOWNLOADING"
#define BIN_DOWNLOADED "BIN_DOWNLOADED"
#define BIN_DOWNLOAD_FAILED "BIN_DOWNLOAD_FAILED"
#define UPDATING "UPDATING"
#define UPDATED "UPDATED"
#define UPDATE_FAILED "UPDATE_FAILED"
#define RESTARTING "RESTARTING"
#define STARTED "STARTED"

#include "Secrets.h"
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
char *wifiSSID = SECRET_SSID;
char *wifiPassword = SECRET_PASS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "puffin.rmq2.cloudamqp.com";
int port = 1883;

const char topic[]  = "main/update";

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  Serial.begin(115200);
  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }
  connectWifi(wifiSSID, wifiPassword);
  checkInternet(wifiClient, wifiSSID, wifiPassword);

  mqttClient.setId("clientId");
  mqttClient.setUsernamePassword(MQTT_USERNAME, MQTT_PASS);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(topic);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);
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
    Serial.print("Parsed json: ");
    Serial.println(jsonMessageString);
    String url = getJsonStringProperty(jsonMessageString, "url");
    if (url != "") {
      Json response;
      response.cloneProperty("id", jsonMessageString);
      response.cloneProperty("commit", jsonMessageString);
      response.addStringProperty("stage", BIN_URL_RECEIVED);
      mqttClient.beginMessage(topic);
      mqttClient.print(response.getJson());
      mqttClient.endMessage();
      // mock up
      delay(1000);
      response.patchStringProperty("stage", BIN_DOWNLOADING);
      mqttClient.beginMessage(topic);
      mqttClient.print(response.getJson());
      mqttClient.endMessage();
      delay(1000);
      response.patchStringProperty("stage", BIN_DOWNLOADED);
      mqttClient.beginMessage(topic);
      mqttClient.print(response.getJson());
      mqttClient.endMessage();
      delay(1000);
      response.patchStringProperty("stage", UPDATING);
      mqttClient.beginMessage(topic);
      mqttClient.print(response.getJson());
      mqttClient.endMessage();
      delay(1000);
      response.patchStringProperty("stage", UPDATED);
      mqttClient.beginMessage(topic);
      mqttClient.print(response.getJson());
      mqttClient.endMessage();
      delay(1000);
      response.patchStringProperty("stage", RESTARTING);
      mqttClient.beginMessage(topic);
      mqttClient.print(response.getJson());
      mqttClient.endMessage();
      delay(1000);
      response.patchStringProperty("stage", STARTED);
      mqttClient.beginMessage(topic);
      mqttClient.print(response.getJson());
      mqttClient.endMessage();
    }
  }

  // do something else
}
