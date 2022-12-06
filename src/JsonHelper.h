#ifndef JsonHelper_h
#define JsonHelper_h
#include "Arduino.h"

extern String parseJson(String payload);
extern String getValue(String payload, String key);
extern String addProperty(String payload, String key, String value);
extern String modifyValue(String payload, String key, String value);

#endif