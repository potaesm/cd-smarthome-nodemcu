#ifndef JsonHelper_h
#define JsonHelper_h
#include "Arduino.h"

String parseJson(String payload)
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
    return payload.substring(startJsonIndex, endJsonIndex + 1);
  }
  else
  {
    return "{}";
  }
}

String getProperty(String payload, String key)
{
  unsigned short childSkipCounter = 0;
  unsigned short splitIndex = 0;
  String returnValue = "";
  String data = "";
  unsigned short dataLength = payload.length();
  if (dataLength != 0)
  {
    if ((payload.charAt(0) == '{' && payload.charAt(dataLength - 1) == '}'))
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
    if (currentChar == '{')
    {
      childSkipCounter++;
    }
    if (currentChar == '}')
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
    if (returnValue.substring(1, returnValue.indexOf(':') - 1) == key)
    {
      returnValue = returnValue.substring(returnValue.indexOf(':') + 1, returnValue.length());
      break;
    }
    else if (i == dataLength - 1)
    {
      return "";
      break;
    }
  }
  return returnValue.substring(1, returnValue.length() - 1);
}

String addProperty(String payload, String key, String value)
{
  String output = "{";
  String extractedJson = payload.substring(1, payload.length() - 1);
  if (extractedJson.length() == 0)
  {
    output += "\"" + key + "\":" + "\"" + value + "\"";
  }
  else
  {
    output += extractedJson;
    output += ",\"" + key + "\":" + "\"" + value + "\"";
  }
  output += "}";
  return output;
}

String updateProperty(String payload, String key, String value)
{
  unsigned short childSkipCounter = 0;
  unsigned short splitIndex = 0;
  unsigned short startEditIndex = 0;
  unsigned short endEditIndex = 0;
  unsigned short dataLength = payload.length();
  String currentValue = "";
  String data = "";
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
          if (currentValue.substring(1, currentValue.indexOf(':') - 1) == key)
          {
            startEditIndex = splitIndex;
            endEditIndex = i;
          }
        }
        else
        {
          currentValue = data.substring(splitIndex + 1, i);
          if (currentValue.substring(1, currentValue.indexOf(':') - 1) == key)
          {
            startEditIndex = splitIndex + 1;
            endEditIndex = i;
          }
        }
        splitIndex = i;
      }
      if (i == dataLength - 1)
      {
        currentValue = data.substring(splitIndex + 1, dataLength);
        if (currentValue.substring(1, currentValue.indexOf(':') - 1) == key)
        {
          startEditIndex = splitIndex + 1;
          endEditIndex = dataLength;
        }
      }
    }
    if (endEditIndex > startEditIndex)
    {
      String firstPartOriginalData = data.substring(0, startEditIndex);
      String lastPartOriginalData = data.substring(endEditIndex, dataLength);
      if (value == "")
      {
        firstPartOriginalData = (endEditIndex == dataLength) ? data.substring(0, startEditIndex - 1) : firstPartOriginalData;
        return "{" + firstPartOriginalData + data.substring(endEditIndex + 1, dataLength) + "}";
      }
      return "{" + firstPartOriginalData + "\"" + key + "\":" + "\"" + value + "\"" + lastPartOriginalData + "}";
    }
    else
    {
      return "{" + data.substring(0, dataLength) + "," + "\"" + key + "\":" + "\"" + value + "\"" + "}";
    }
  }
  else
  {
    return "{\"" + key + "\":" + "\"" + value + "\"" + "}";
  }
}

#endif