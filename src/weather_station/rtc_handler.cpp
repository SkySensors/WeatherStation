#include "rtc_handler.hpp"
#include "log_handler.hpp"

void RTCHandler::Initialize()
{
    Wire.begin();
    SynchronizeWithServerTime();
}

RTCHandler::RTCHandler(SkySensorsAPIHandler &skySensorsAPI)
    : skySensorsAPIHandler(skySensorsAPI)
{
}

void RTCHandler::SynchronizeWithServerTime()
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

DateTime RTCHandler::GetDateTimeNow()
{
    return rtcLib.now();
}

int RTCHandler::GetCurrentHourInSeconds()
{
    DateTime now = GetDateTimeNow();
    return now.minute() * 60 + now.second();
}
