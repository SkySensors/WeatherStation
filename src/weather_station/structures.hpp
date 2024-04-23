#pragma once
#include <ArduinoJson.h>

enum class SensorType
{
    Temperature = 0,
    Humidity = 1,
    Light = 2,
};

struct SensorValue
{
    long unixTime;
    float value;
};

struct SensorValues
{
    String macAddress;
    SensorType sensorType;
    SensorValue* sensorValues;
    int sensorValueAmount;

public:
    void ToJson(String& output)
    {
        JsonDocument doc;
        doc["macAddress"] = macAddress;
        doc["type"] = (int)sensorType;
        
        JsonDocument arrayDoc;
        JsonArray array = arrayDoc.to<JsonArray>();
        for (int i = 0; i < sensorValueAmount; i++)
        {
            if (isnan(sensorValues[i].value))
            {
                continue;
            }
            JsonObject object = array.createNestedObject();
            object["unixTime"] = sensorValues[i].unixTime;
            object["value"] = sensorValues[i].value;
        }
        doc["sensorValues"] = arrayDoc;  
        serializeJson(doc, output);     
    }
};

struct TimeSlot
{
    int secondToTriggerAt;
    int intervalSeconds;

public:
    void FromJson(String& json)
    {
        JsonDocument doc;
        deserializeJson(doc, json);
        secondToTriggerAt = doc["secondsNumber"];
        intervalSeconds = doc["intervalSeconds"];
    }
};

struct SensorData
{
    SensorType sensorType;
};

struct GpsLocation
{
    double latitude;
    double longitude;
};

struct WeatherStation
{
    String macAddress;
    GpsLocation gpsLocation;
    SensorData* sensorDataArray;
    int sensorAmount;

public:
    void ToJson(String& output)
    {
        JsonDocument doc;
        doc["macAddress"] = macAddress;
        doc["gpsLocation"]["latitude"] = gpsLocation.latitude;
        doc["gpsLocation"]["longitude"] = gpsLocation.longitude;
        
        JsonDocument arrayDoc;
        JsonArray array = arrayDoc.to<JsonArray>();
        for (int i = 0; i < sensorAmount; i++)
        {
            JsonObject object = array.createNestedObject();
            object["type"] = (int)sensorDataArray[i].sensorType;
        }
        doc["sensors"] = arrayDoc;

        serializeJson(doc, output);
    }
};
