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

#define SENSOR_STORE "/sensor.txt"
#define REPORT_STORE "/report.txt"

String readData(String filePath = SENSOR_STORE)
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

void writeData(String data, String filePath = SENSOR_STORE)
{
  File file = LittleFS.open(filePath, "w");
  file.print(data);
  file.close();
  delay(1);
}

void removeData(String filePath = SENSOR_STORE)
{
  LittleFS.remove(filePath);
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
    sensor = DS18B20_SENSOR;
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

#define MAX_REPORT_NUMBER 3
#define MAX_SENSOR_ATTEMPT_NUMER 24

void reportDS18B20()
{
  OneWire oneWire(D3);
  DallasTemperature DS18B20(&oneWire);
  DS18B20.begin();
  DS18B20.requestTemperatures();
  float tempC = DS18B20.getTempCByIndex(0);
  float tempF = DS18B20.getTempFByIndex(0);
  accTempC += tempC;
  accTempF += tempF;
  reportNumber++;
  if (reportNumber == MAX_REPORT_NUMBER) {
    float avgTempC = round(accTempC / MAX_REPORT_NUMBER);
    float avgTempF = round(accTempF / MAX_REPORT_NUMBER);
    String avgTempCelsius = String(avgTempC) + "ºC";
    String avgTempFahrenheit = String(avgTempF) + "ºF";
    Serial.println();
    Serial.print("Average temperature: ");
    Serial.print(avgTempCelsius);
    Serial.print(", ");
    Serial.println(avgTempFahrenheit);
    writeData(String(avgTempCelsius), REPORT_STORE);
    accTempC = 0.0f;
    accTempF = 0.0f;
  }
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
      String avgHeartRate = String(avgBPM) + " bpm";
      String avgBloodOxygenLevel = String(avgSpO2) + "%";
      Serial.println();
      Serial.print("Average heart rate: ");
      Serial.println(avgHeartRate);
      Serial.print("Average blood oxygen level: ");
      Serial.println(avgBloodOxygenLevel);
      // Send PPP data
      String data = "{}";
      String avgTempCelsius = readData(REPORT_STORE);
      data = addProperty(data, "id", DEVICE_ID);
      data = addProperty(data, "temp", avgTempCelsius);
      data = addProperty(data, "heart_rate", avgHeartRate);
      data = addProperty(data, "blood_oxygen", avgBloodOxygenLevel);
      data = addProperty(data, "email", "21mcs020@nith.ac.in");
      data = addProperty(data, "name", "Suthinan Musitmani");
      data = addProperty(data, "report_length", "5");
      sendMQTTMessage(data, "app/data");
      removeData(REPORT_STORE);
      accBPM = 0.0f;
      accSpO2 = 0.0f;
      sensorAttemptNumber = 0;
    }
  }
  else
  {
    reportNumber = 0;
    accBPM = 0.0f;
    accSpO2 = 0.0f;
    if (sensorAttemptNumber == MAX_SENSOR_ATTEMPT_NUMER) {
      Serial.println("Failed to get heart rate and blood oxygen level");
      reportNumber = MAX_REPORT_NUMBER;
      sensorAttemptNumber = 0;
    } else {
      sensorAttemptNumber++;
    }
  }
  prevoiusBPM = BPM;
}

#endif