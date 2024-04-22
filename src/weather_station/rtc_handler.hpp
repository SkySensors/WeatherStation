#pragma once
#include "def.hpp"
#include "sky_sensors_api_handler.hpp"

class RTCHandler
{
private:
    SkySensorsAPIHandler& skySensorsAPIHandler;

private:
    RTClib rtcLib;
    DS3231 ds3231;

public:
    RTCHandler(SkySensorsAPIHandler& skySensorsAPIHandler);

public:
    // Used initialize the RTC handler
    void Initialize();

    // Used to synchronize the RTC with the server time
    void SynchronizeWithServerTime();

public:
    // Returns current DateTime
    DateTime GetDateTimeNow();

    // Returns current times hour in seconds 
    int GetCurrentHourInSeconds();
};