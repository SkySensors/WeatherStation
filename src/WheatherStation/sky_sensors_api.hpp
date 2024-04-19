#pragma once
#include "def.hpp"

class SkySensorsAPI
{
private:
    HttpClient& client;

public:
    SkySensorsAPI(HttpClient& client);

public:
    TimeSlot MakeWeatherStationHandshake(WheatherStation& wheatherStation);
    void AddSensorValues(const char* macAddress, Sensor& sensor);
    unsigned long GetCurrentUnixTime();
};