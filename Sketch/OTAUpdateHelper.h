#ifndef OTAUpdateHelper_h
#define OTAUpdateHelper_h
#include "Arduino.h"
#include "Config.h"

String previousMQTTMessage = "";

void handleMQTTUpdateMessage(void (*function)(String commit, String url), char* topic, byte* payload, unsigned int payloadLength);
void connectMQTTBroker(void (*function)(), const char id[], const char user[], const char pass[], const char topic[], const byte qos);

void handleMQTTUpdateMessage(void (*function)(String commit, String url), char* topic, byte* payload, unsigned int payloadLength){
  if (payloadLength && String(topic) == MQTT_TOPIC) {
    String message = parseJson(String((char*) payload));
    bool isDuplicated = message == previousMQTTMessage;
    if (!isDuplicated) {
      previousMQTTMessage = message;
    }
#if defined(DEVMODE)
    String printMessage = "Received message: ";
    if (isDuplicated) {
      printMessage = "Received duplicated message: ";
    }
    Serial.println(printMessage + message);
#endif
    String id = getProperty(message, "id");
    String commit = getProperty(message, "commit");
    String url = getProperty(message, "url");
    bool isAllowed = (id == "") ? true : (id == DEVICE_ID) ? true : false;
    if (!isDuplicated && url != "" && isAllowed) {
      // Handle update
      (*function)(commit, url);
    }
  }
}

void connectMQTTBroker(void (*function)(), const char id[] = DEVICE_ID, const char user[] = MQTT_USERNAME, const char pass[] = MQTT_PASS, const char topic[] = MQTT_TOPIC, const byte qos = MQTT_SUBSCRIBE_QOS) {
  byte mqttAttemptNumber = 0;
#if defined(DEVMODE)
    Serial.println("Attempting MQTT connection");
#endif
  while (!mqttClient.connected()) {
    mqttAttemptNumber++;
#if defined(DEVMODE)
    Serial.print(".");
#endif
    if (mqttClient.connect(id, user, pass)) {
#if defined(DEVMODE)
      Serial.println();
      Serial.println("MQTT broker connected");
#endif
    } else if (mqttAttemptNumber == 10) {
      // Restart
      (*function)();
    }
  }
  mqttClient.subscribe(topic, qos);
#if defined(DEVMODE)
  Serial.println("Subscribed to topic " + String(topic));
#endif
}

void sendMQTTMessage(String message, char topic[] = MQTT_TOPIC, bool retained = MQTT_MESSAGE_RETAINED) {
#if defined(DEVMODE)
  Serial.println("Send message: " + message);
#endif
  mqttClient.publish(topic, message.c_str(), retained);
}

#endif