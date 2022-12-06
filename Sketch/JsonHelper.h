#ifndef JsonHelper_h
#define JsonHelper_h
#include "Arduino.h"

extern String parseJson(String payload);
extern String getProperty(String payload, String key);
extern String addProperty(String payload, String key, String value);
extern String updateProperty(String payload, String key, String value);

#endif