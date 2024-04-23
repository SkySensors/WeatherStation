#pragma once
#include "def.hpp"
#include "sky_sensors_api_handler.hpp"

class RtcHandler
{
private:
    SkySensorsAPIHandler& skySensorsAPIHandler;

private:
    RTClib rtcLib;
    DS3231 ds3231;

public:
    RtcHandler(SkySensorsAPIHandler& skySensorsAPIHandler)
        : skySensorsAPIHandler(skySensorsAPIHandler)
    {
    }   

public:
    // Used initialize the RTC handler
    void Initialize()
    {
        Wire.begin();
        SynchronizeWithServerTime();
    }

    // Used to synchronize the RTC with the server time
    void SynchronizeWithServerTime()
    {
        unsigned long unixTime = skySensorsAPIHandler.GetServerTimeInUnixFormat();
        if (unixTime == -1)
        {
            LogHandler.LogError(100, 10, "Failed to synchronize RTC clock to server time.");
            return;
        }
        ds3231.setEpoch(unixTime, true);
        LogHandler.LogInfo("RTC clock has been synchronized with server time: " + String(unixTime));
    }

public:
    // Returns current DateTime
    DateTime GetDateTimeNow()
    {
        return rtcLib.now();
    }

    // Returns current times hour in seconds 
    int GetCurrentHourInSeconds()
    {
        DateTime now = GetDateTimeNow();
        return now.minute() * 60 + now.second();
    }
};
