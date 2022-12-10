#ifndef Utils_h
#define Utils_h
#include "Arduino.h"

unsigned long callbackRoutine(void (*function)(), unsigned long previousMillis, const long interval);

unsigned long callbackRoutine(void (*function)(), unsigned long previousMillis, const long interval)
{
  if (millis() - previousMillis >= interval)
  {
    (*function)();
    previousMillis = millis();
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

#define STORE "/store.txt"

String readData(String filePath = STORE)
{
  String data = "";
  File file = LittleFS.open(filePath, "r");
  if (!file)
    return data;
  while (file.available())
    data.concat(file.readString());
  file.close();
  return data;
}

void writeData(String data, String filePath = STORE)
{
  File file = LittleFS.open(filePath, "w");
  file.print(data);
  file.close();
  delay(1);
}

int beginSensor()
{
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    softwareReset();
  }
  int sensor = readData().toInt();
  if (sensor == NO_SENSOR || sensor == LIMIT_SENSOR)
    sensor = MAX30100_SENSOR;
  switch (sensor)
  {
  case MAX30100_SENSOR:
  {
    pinMode(D0, OUTPUT);
    delay(100);
    if (!POX.begin())
    {
      Serial.println("Pulse oximeter failed to begin");
      softwareReset();
    }
    delay(100);
    Serial.println("MAX30100 is working, hold your finger still");
    break;
  }
  default:
    break;
  }
  return sensor;
}

#define MAX_REPORT_NUMBER 5

void reportDS18B20()
{
  OneWire oneWire(D3);
  DallasTemperature DS18B20(&oneWire);
  DS18B20.begin();
  DS18B20.requestTemperatures();
  float temperatureC = DS18B20.getTempCByIndex(0);
  float temperatureF = DS18B20.getTempFByIndex(0);
  Serial.println("DS18B20");
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");
  reportNumber = MAX_REPORT_NUMBER;
}

void reportMAX30100()
{
  float BPM = POX.getHeartRate();
  float SpO2 = POX.getSpO2();
  if (BPM != 0.0f && abs(BPM - prevoiusBPM) < 5.0f && SpO2 != 0.0f && SpO2 < 100.0f)
  {
    Serial.print(".");
    accBPM += BPM;
    accSpO2 += SpO2;
    reportNumber++;
    if (reportNumber == MAX_REPORT_NUMBER)
    {
      float avgBPM = round(accBPM / MAX_REPORT_NUMBER);
      float avgSpO2 = round(accSpO2 / MAX_REPORT_NUMBER);
      Serial.println();
      Serial.print("Average BPM: ");
      Serial.println(avgBPM);
      Serial.print("Average SpO2: ");
      Serial.println(avgSpO2);
    }
  }
  else
  {
    reportNumber = 0;
    accBPM = 0.0f;
    accSpO2 = 0.0f;
  }
  prevoiusBPM = BPM;
}

#endif