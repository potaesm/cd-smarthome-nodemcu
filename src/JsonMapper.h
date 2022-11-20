#ifndef JsonMapper_h
#define JsonMapper_h
#include "Arduino.h"

class Json
{
public:
    Json();
    void setJson(String payload);
    void addProperty(String key, String value, bool isString = false);
    void addStringProperty(String key, String value);
    void addNumberProperty(String key, float value);
    void addBooleanProperty(String key, bool value);
    void cloneProperty(String targetKey, String source, String sourceKey = "");
    void patchProperty(String key, String value, bool isString = false);
    void patchStringProperty(String key, String value);
    void patchNumberProperty(String key, float value);
    void patchBooleanProperty(String key, bool value);
    void removeProperty(String key);
    String getJson();

private:
    String jsonString = "{}";
};

class List
{
public:
    List();
    void setList(String payload);
    void addValue(String value, bool isString = false);
    void addStringValue(String value);
    void addNumberValue(float value);
    void addBooleanValue(bool value);
    void patchValue(unsigned short index, String value, bool isString = false);
    void patchStringValue(unsigned short index, String value);
    void patchNumberValue(unsigned short index, float value);
    void patchBooleanValue(unsigned short index, bool value);
    void removeValue(unsigned short index);
    String getList();

private:
    String listString = "[]";
};

class JsonList
{
public:
    JsonList();
    void setJsonList(String payload);
    String getJsonList();
    String getJson(unsigned short index);
    unsigned short length();

private:
    String jsonListString = "";
    unsigned short jsonLength = 0;
    ::List jsonList;
};

extern String patchListValue(String payload, unsigned short index, String value, bool isString = false);
extern String patchListNumberValue(String payload, unsigned short index, float value);
extern String patchListBooleanValue(String payload, unsigned short index, bool value);
extern String patchListStringValue(String payload, unsigned short index, String value);
extern String patchJsonProperty(String payload, String key, String value, bool isString = false);
extern String patchJsonNumberProperty(String payload, String key, float value);
extern String patchJsonBooleanProperty(String payload, String key, bool value);
extern String patchJsonStringProperty(String payload, String key, String value);
extern String removeListValue(String payload, unsigned short index);
extern String removeJsonProperty(String payload, String key);
extern String getListValue(String payload, unsigned short index);
extern float getListNumberValue(String payload, unsigned short index);
extern bool getListBooleanValue(String payload, unsigned short index);
extern String getListStringValue(String payload, unsigned short index);
extern String getJsonProperty(String payload, String key);
extern float getJsonNumberProperty(String payload, String key);
extern bool getJsonBooleanProperty(String payload, String key);
extern String getJsonStringProperty(String payload, String key);
extern float parseNumber(String payload);
extern bool parseBoolean(String payload);
extern String parseString(String payload);

#endif