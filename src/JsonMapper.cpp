#include "Arduino.h"
#include "JsonMapper.h"

String modifyValue(String payload, unsigned short index, String key, String value, bool isSetByIndex, bool isString)
{
  unsigned short childSkipCounter = 0;
  unsigned short indexCounter = 0;
  unsigned short splitIndex = 0;
  unsigned short startEditIndex = 0;
  unsigned short endEditIndex = 0;
  unsigned short dataLength = payload.length();
  String currentValue = "";
  String data = "";
  String modifer = isString ? "\"" : "";
  if (dataLength > 2)
  {
    data = payload.substring(1, dataLength - 1);
    dataLength = data.length();
    for (unsigned short i = 0; i < dataLength; i++)
    {
      char currentChar = data.charAt(i);
      if (currentChar == '{' || currentChar == '[')
      {
        childSkipCounter++;
      }
      if (currentChar == '}' || currentChar == ']')
      {
        childSkipCounter--;
      }
      if (childSkipCounter == 0 && currentChar == ',')
      {
        if (splitIndex == 0)
        {
          currentValue = data.substring(splitIndex, i);
          if (isSetByIndex)
          {
            if (index == indexCounter)
            {
              startEditIndex = splitIndex;
              endEditIndex = i;
            }
          }
          else
          {
            if (currentValue.substring(1, currentValue.indexOf(':') - 1) == key)
            {
              startEditIndex = splitIndex;
              endEditIndex = i;
            }
          }
        }
        else
        {
          currentValue = data.substring(splitIndex + 1, i);
          if (isSetByIndex)
          {
            if (index == indexCounter)
            {
              startEditIndex = splitIndex + 1;
              endEditIndex = i;
            }
          }
          else
          {
            if (currentValue.substring(1, currentValue.indexOf(':') - 1) == key)
            {
              startEditIndex = splitIndex + 1;
              endEditIndex = i;
            }
          }
        }
        splitIndex = i;
        indexCounter++;
      }
      if (i == dataLength - 1)
      {
        currentValue = data.substring(splitIndex + 1, dataLength);
        if (isSetByIndex)
        {
          if (index == indexCounter)
          {
            startEditIndex = splitIndex + 1;
            endEditIndex = dataLength;
          }
        }
        else
        {
          if (currentValue.substring(1, currentValue.indexOf(':') - 1) == key)
          {
            startEditIndex = splitIndex + 1;
            endEditIndex = dataLength;
          }
        }
      }
    }
    if (endEditIndex > startEditIndex)
    {
      String firstPartOriginalData = data.substring(0, startEditIndex);
      String lastPartOriginalData = data.substring(endEditIndex, dataLength);
      if (isSetByIndex)
      {
        if (value == "")
        {
          firstPartOriginalData = (endEditIndex == dataLength) ? data.substring(0, startEditIndex - 1) : firstPartOriginalData;
          return "[" + firstPartOriginalData + data.substring(endEditIndex + 1, dataLength) + "]";
        }
        return "[" + firstPartOriginalData + modifer + value + modifer + lastPartOriginalData + "]";
      }
      else
      {
        if (value == "")
        {
          firstPartOriginalData = (endEditIndex == dataLength) ? data.substring(0, startEditIndex - 1) : firstPartOriginalData;
          return "{" + firstPartOriginalData + data.substring(endEditIndex + 1, dataLength) + "}";
        }
        return "{" + firstPartOriginalData + "\"" + key + "\":" + modifer + value + modifer + lastPartOriginalData + "}";
      }
    }
    else
    {
      if (isSetByIndex)
      {
        return "[" + data.substring(0, dataLength) + "," + modifer + value + modifer + "]";
      }
      else
      {
        return "{" + data.substring(0, dataLength) + "," + "\"" + key + "\":" + modifer + value + modifer + "}";
      }
    }
  }
  else
  {
    if (isSetByIndex)
    {
      return "[" + modifer + value + modifer + "]";
    }
    else
    {
      return "{\"" + key + "\":" + modifer + value + modifer + "}";
    }
  }
}

String patchListValue(String payload, unsigned short index, String value, bool isString)
{
  return modifyValue(payload, index, "", value, true, isString);
}

String patchListNumberValue(String payload, unsigned short index, float value)
{
  return modifyValue(payload, index, "", String(value), true, false);
}

String patchListBooleanValue(String payload, unsigned short index, bool value)
{
  String booleanString = value ? "true" : "false";
  return modifyValue(payload, index, "", booleanString, true, false);
}

String patchListStringValue(String payload, unsigned short index, String value)
{
  return modifyValue(payload, index, "", value, true, true);
}

String removeListValue(String payload, unsigned short index)
{
  return patchListStringValue(payload, index, "");
}

String patchJsonProperty(String payload, String key, String value, bool isString)
{
  return modifyValue(payload, 0, key, value, false, isString);
}

String patchJsonNumberProperty(String payload, String key, float value)
{
  return modifyValue(payload, 0, key, String(value), false, false);
}

String patchJsonBooleanProperty(String payload, String key, bool value)
{
  String booleanString = value ? "true" : "false";
  return modifyValue(payload, 0, key, booleanString, false, false);
}

String patchJsonStringProperty(String payload, String key, String value)
{
  return modifyValue(payload, 0, key, value, false, true);
}

String removeJsonProperty(String payload, String key)
{
  return patchJsonStringProperty(payload, key, "");
}

String getValue(String payload, unsigned short index, String key, bool isFindByIndex)
{
  unsigned short childSkipCounter = 0;
  unsigned short indexCounter = 0;
  unsigned short splitIndex = 0;
  String returnValue = "";
  String data = "";
  bool breakInvalid = false;
  unsigned short dataLength = payload.length();
  if (dataLength != 0)
  {
    if ((payload.charAt(0) == '{' && payload.charAt(dataLength - 1) == '}') || payload.charAt(0) == '[' && payload.charAt(dataLength - 1) == ']')
    {
      data = payload.substring(1, dataLength - 1);
      dataLength = data.length();
    }
    else
    {
      return returnValue;
    }
  }
  else
  {
    return returnValue;
  }
  for (unsigned short i = 0; i < dataLength; i++)
  {
    char currentChar = data.charAt(i);
    if (currentChar == '{' || currentChar == '[')
    {
      childSkipCounter++;
    }
    if (currentChar == '}' || currentChar == ']')
    {
      childSkipCounter--;
    }
    if (childSkipCounter == 0 && currentChar == ',')
    {
      if (splitIndex == 0)
      {
        returnValue = data.substring(splitIndex, i);
      }
      else
      {
        returnValue = data.substring(splitIndex + 1, i);
      }
      splitIndex = i;
      indexCounter++;
    }
    if (i == dataLength - 1)
    {
      if (splitIndex == 0)
      {
        returnValue = data.substring(splitIndex, dataLength);
      }
      else
      {
        returnValue = data.substring(splitIndex + 1, dataLength);
      }
    }
    if (isFindByIndex)
    {
      if (indexCounter > index)
      {
        break;
      }
      if ((i == dataLength - 1) && (indexCounter < index))
      {
        returnValue = "";
        break;
      }
    }
    else
    {
      if (returnValue.substring(1, returnValue.indexOf(':') - 1) == key)
      {
        returnValue = returnValue.substring(returnValue.indexOf(':') + 1, returnValue.length());
        break;
      }
      else if (i == dataLength - 1)
      {
        returnValue = "";
        break;
      }
    }
  }
  return returnValue;
}

String getListValue(String payload, unsigned short index)
{
  return getValue(payload, index, "", true);
}

float getListNumberValue(String payload, unsigned short index)
{
  return parseNumber(getValue(payload, index, "", true));
}

bool getListBooleanValue(String payload, unsigned short index)
{
  return parseBoolean(getValue(payload, index, "", true));
}

String getListStringValue(String payload, unsigned short index)
{
  return parseString(getValue(payload, index, "", true));
}

String getJsonProperty(String payload, String key)
{
  return getValue(payload, 0, key, false);
}

float getJsonNumberProperty(String payload, String key)
{
  return parseNumber(getValue(payload, 0, key, false));
}

bool getJsonBooleanProperty(String payload, String key)
{
  return parseBoolean(getValue(payload, 0, key, false));
}

String getJsonStringProperty(String payload, String key)
{
  return parseString(getValue(payload, 0, key, false));
}

float parseNumber(String payload)
{
  return payload.toFloat();
}

bool parseBoolean(String payload)
{
  String lowerCasePayload = payload;
  lowerCasePayload.toLowerCase();
  if (lowerCasePayload == "true")
  {
    return true;
  }
  else
  {
    return false;
  }
}

String parseString(String payload)
{
  return payload.substring(1, payload.length() - 1);
}

Json::Json()
{
}

List::List()
{
}

JsonList::JsonList()
{
}

void Json::setJson(String payload)
{
  unsigned short childSkipCounter = 0;
  unsigned short startJsonIndex = 0;
  bool isStartJsonIndexSet = false;
  unsigned short endJsonIndex = 0;
  bool isEndJsonIndexSet = false;
  for (unsigned short i = 0; i < payload.length(); i++)
  {
    char currentChar = payload.charAt(i);
    if (currentChar == '{' || currentChar == '[')
    {
      if (currentChar == '{' && !isStartJsonIndexSet)
      {
        startJsonIndex = i;
        isStartJsonIndexSet = true;
      }
      childSkipCounter++;
    }
    if (currentChar == '}' || currentChar == ']')
    {
      childSkipCounter--;
    }
    if (childSkipCounter == 0)
    {
      if (currentChar == '}' && !isEndJsonIndexSet)
      {
        endJsonIndex = i;
        isEndJsonIndexSet = true;
        break;
      }
    }
  }
  if (isStartJsonIndexSet && isEndJsonIndexSet)
  {
    jsonString = payload.substring(startJsonIndex, endJsonIndex + 1);
  }
  else
  {
    jsonString = "{}";
  }
}

void Json::addProperty(String key, String value, bool isString)
{
  String output = "{";
  String modifer = isString ? "\"" : "";
  String extractedJson = jsonString.substring(1, jsonString.length() - 1);
  if (extractedJson.length() == 0)
  {
    output += "\"" + key + "\":" + modifer + value + modifer;
  }
  else
  {
    output += extractedJson;
    output += ",\"" + key + "\":" + modifer + value + modifer;
  }
  output += "}";
  jsonString = output;
}

void Json::addStringProperty(String key, String value)
{
  Json::addProperty(key, value, true);
}

void Json::addNumberProperty(String key, float value)
{
  Json::addProperty(key, String(value), false);
}

void Json::addBooleanProperty(String key, bool value)
{
  String booleanString = value ? "true" : "false";
  Json::addProperty(key, booleanString, false);
}

void Json::cloneProperty(String targetKey, String source, String sourceKey)
{
  if (sourceKey.length() > 0)
  {
    Json::addProperty(targetKey, getJsonProperty(source, sourceKey), false);
  }
  else
  {
    Json::addProperty(targetKey, getJsonProperty(source, targetKey), false);
  }
}

void Json::patchProperty(String key, String value, bool isString)
{
  jsonString = patchJsonProperty(jsonString, key, value, isString);
}

void Json::patchStringProperty(String key, String value)
{
  jsonString = patchJsonStringProperty(jsonString, key, value);
}

void Json::patchNumberProperty(String key, float value)
{
  jsonString = patchJsonNumberProperty(jsonString, key, value);
}

void Json::patchBooleanProperty(String key, bool value)
{
  jsonString = patchJsonBooleanProperty(jsonString, key, value);
}

void Json::removeProperty(String key)
{
  jsonString = removeJsonProperty(jsonString, key);
}

String Json::getJson()
{
  return jsonString;
}

void List::setList(String payload)
{
  listString = payload;
}

void List::addValue(String value, bool isString)
{
  String output = "[";
  String modifer = isString ? "\"" : "";
  String extractedList = listString.substring(1, listString.length() - 1);
  if (extractedList.length() == 0)
  {
    output += modifer + value + modifer;
  }
  else
  {
    output += extractedList;
    output += "," + modifer + value + modifer;
  }
  output += "]";
  listString = output;
}

void List::addStringValue(String value)
{
  List::addValue(value, true);
}

void List::addNumberValue(float value)
{
  List::addValue(String(value), false);
}

void List::addBooleanValue(bool value)
{
  String booleanString = value ? "true" : "false";
  List::addValue(booleanString, false);
}

void List::patchValue(unsigned short index, String value, bool isString)
{
  listString = patchListValue(listString, index, value, isString);
}

void List::patchStringValue(unsigned short index, String value)
{
  listString = patchListStringValue(listString, index, value);
}

void List::patchNumberValue(unsigned short index, float value)
{
  listString = patchListNumberValue(listString, index, value);
}

void List::patchBooleanValue(unsigned short index, bool value)
{
  listString = patchListBooleanValue(listString, index, value);
}

void List::removeValue(unsigned short index)
{
  listString = removeListValue(listString, index);
}

String List::getList()
{
  return listString;
}

void JsonList::setJsonList(String payload)
{
  unsigned short childSkipCounter = 0;
  unsigned short indexCounter = 0;
  unsigned short splitIndex = 0;
  String data = "";
  unsigned short dataLength = payload.length();
  if (dataLength != 0)
  {
    if ((payload.charAt(0) == '{' && payload.charAt(dataLength - 1) == '}') || payload.charAt(0) == '[' && payload.charAt(dataLength - 1) == ']')
    {
      jsonListString = payload;
      data = payload.substring(1, dataLength - 1);
      dataLength = data.length();
    }
    else
    {
      jsonListString = "";
    }
  }
  else
  {
    jsonListString = "";
  }
  for (unsigned short i = 0; i < dataLength; i++)
  {
    char currentChar = data.charAt(i);
    if (currentChar == '{' || currentChar == '[')
    {
      childSkipCounter++;
    }
    if (currentChar == '}' || currentChar == ']')
    {
      childSkipCounter--;
    }
    if (childSkipCounter == 0 && currentChar == ',')
    {
      if (splitIndex == 0)
      {
        jsonList.addValue(data.substring(splitIndex, i));
      }
      else
      {
        jsonList.addValue(data.substring(splitIndex + 1, i));
      }
      splitIndex = i;
      indexCounter++;
    }
    if (i == dataLength - 1)
    {
      if (splitIndex == 0)
      {
        jsonList.addValue(data.substring(splitIndex, dataLength));
      }
      else
      {
        jsonList.addValue(data.substring(splitIndex + 1, dataLength));
      }
      indexCounter++;
    }
  }
  jsonLength = indexCounter;
}

String JsonList::getJsonList()
{
  return jsonListString;
}

String JsonList::getJson(unsigned short index)
{
  if (index < jsonLength)
  {
    return getListValue(jsonList.getList(), index);
  }
  else
  {
    return "";
  }
}

unsigned short JsonList::length()
{
  return jsonLength;
}