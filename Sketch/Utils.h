#ifndef Utils_h
#define Utils_h
#include "Arduino.h"

unsigned long callbackRoutine(void (*function)(), unsigned long previousMillis, const long interval);

unsigned long callbackRoutine(void (*function)(), unsigned long previousMillis, const long interval)
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    (*function)();
  }
  return previousMillis;
}

void softwareReset()
{
  delay(500);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(500);
  ESP.restart();
}

#endif