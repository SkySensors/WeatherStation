#pragma once
#include "def.hpp"

#define MAX_SENSORS 3
#define MAX_SENSOR_VALUE_BUFFER_SIZE 80


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

struct Sensor
{
    SensorType sensorType;
    SensorValue sensorValues[MAX_SENSOR_VALUE_BUFFER_SIZE];
    int sensorValueAmount;
};

struct GpsLocation
{
    float longitude;
    float latitude;
};

struct TimeSlot
{
    int secondNumber;
    int intervalSeconds;

    void FromJson(String json)
    {
        StaticJsonDocument<200> doc;
        deserializeJson(doc, json);
        secondNumber = doc["secondNumber"];
        intervalSeconds = doc["intervalSeconds"];
    }
};

class WheatherStation
{
public:
    char macAddress[18];
    int sensorAmount;
    GpsLocation gpsLocation;
    Sensor sensors[MAX_SENSORS];

public:
    WheatherStation(char macAddress[18], GpsLocation gpsLocation, Sensor* sensors, int sensorAmount = MAX_SENSORS)
        : gpsLocation(gpsLocation), sensorAmount(sensorAmount)
    {
        memcpy(this->macAddress, macAddress, 18);
        memcpy(this->sensors, sensors, sizeof(Sensor) * sensorAmount);
    }

public:
    void ToJson(String& output)
    {
        DynamicJsonDocument doc(256);
        doc["macAddress"] = macAddress;
        doc["gpsLocation"]["longitude"] = gpsLocation.longitude;
        doc["gpsLocation"]["latitude"] = gpsLocation.latitude;
        
        DynamicJsonDocument arrayDoc(256);
        JsonArray array = arrayDoc.to<JsonArray>();
        for (int i = 0; i < sensorAmount; i++)
        {
            JsonObject object = array.createNestedObject();
            object["type"] = (int)sensors[i].sensorType;
        }
        doc["sensors"] = arrayDoc;


        serializeJson(doc, output);
    }
};
