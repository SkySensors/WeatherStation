#include "sky_sensors_api.hpp"
#include <stdlib.h>

SkySensorsAPI::SkySensorsAPI(HttpClient &client)
    : client(client)
{
}

TimeSlot SkySensorsAPI::MakeWeatherStationHandshake(WheatherStation &wheatherStation)
{
    String json = "";
    wheatherStation.ToJson(json);
    String contentType = "application/json";
    String path = "/api/weatherstation/handshake";
    Serial.println(json);
    client.post(path, contentType, json);
    String response = client.responseBody();
    client.stop();
    
    if (response.length() == 0)
    {
        Serial.println(client.responseStatusCode());
        Serial.println("Handshake failed with server");
        return { 30, 60 };
    }
    Serial.println("Made handshake with server");

    TimeSlot timeSlot;
    timeSlot.FromJson(response);
    return timeSlot;
}

void SkySensorsAPI::AddSensorValues(const char* macAddress, Sensor& sensor)
{
    DynamicJsonDocument doc(8192);
    doc["macAddress"] = macAddress;
    doc["type"] = (int)sensor.sensorType;
    
    DynamicJsonDocument arrayDoc(8192);
    JsonArray array = arrayDoc.to<JsonArray>();
    for (int i = 0; i < sensor.sensorValueAmount; i++)
    {
        JsonObject object = array.createNestedObject();
        object["unixTime"] = sensor.sensorValues[i].unixTime;
        object["value"] = sensor.sensorValues[i].value;
    }
    doc["sensorValues"] = arrayDoc;

    String json = "";
    serializeJson(doc, json);
    json = "[" + json + "]";

    Serial.println(json);

    String contentType = "application/json";
    String path = "/api/weatherstation";
    client.post(path, contentType, json);
    int code = client.responseStatusCode();
    client.stop();

    Serial.println(code);
}

unsigned long SkySensorsAPI::GetCurrentUnixTime()
{
    String path = "/api/time";
    client.get(path);
    String response = client.responseBody();
    client.stop();

    if (response.length() == 0)
    {
        Serial.println("Failed to get current time.");
        return -1;
    } 
    return (unsigned long)response.substring(0, 10).toDouble();
}