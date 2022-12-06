#ifndef OTAUpdateHelper_h
#define OTAUpdateHelper_h
#include "Arduino.h"
#include "Config.h"

void handleMQTTMessage(void (*)(String commit, String url));
void connectMQTTBroker(void (*)(), char topic[], byte qos, char broker[], int port);

void handleMQTTMessage(void (*function)(String commit, String url)){
  if (mqttClient.parseMessage() && String(mqttClient.messageTopic()) == MQTT_TOPIC) {
    String message = "";
    while (mqttClient.available()) {
      char currentChar = (char)mqttClient.read();
      message.concat(currentChar);
    }
    message = parseJson(message);
#if defined(DEVMODE)
    Serial.println("Received message: " + message);
#endif
    String id = getProperty(message, "id");
    String commit = getProperty(message, "commit");
    String url = getProperty(message, "url");
    bool isAllowed = (id == "") ? true : (id == DEVICE_ID) ? true : false;
    if (url != "" && isAllowed) {
      (*function)(commit, url);
    }
  }
}

void connectMQTTBroker(void (*function)(), char topic[] = MQTT_TOPIC, byte qos = MQTT_SUBSCRIBE_QOS, char broker[] = MQTT_BROKER, int port = MQTT_PORT) {
  if (!mqttClient.connect(broker, port)) {
#if defined(DEVMODE)
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
#endif
    (*function)();
  }
#if defined(DEVMODE)
  Serial.println("Connected to broker " + String(broker));
#endif
  mqttClient.subscribe(topic, qos);
#if defined(DEVMODE)
  Serial.println("Subscribed to topic " + String(topic));
#endif
}

void sendMQTTMessage(String message, char topic[] = MQTT_TOPIC, bool retained = MQTT_MESSAGE_RETAINED, byte qos = MQTT_MESSAGE_QOS) {
#if defined(DEVMODE)
    Serial.println("Send message: " + message);
#endif
  mqttClient.beginMessage(topic, message.length(), retained, qos);
  mqttClient.print(message);
  mqttClient.endMessage();
}

#endif