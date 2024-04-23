#include "weather_station_handler.hpp"
#include "log_handler.hpp"

WeatherStationHandler::WeatherStationHandler(
    SensorHandler& sh, SkySensorsAPIHandler& ssApi, RTCHandler& rtc, GpsHandler& gps, String macAddress, int sensorValueSize)
    : sensorHandler(sh), skySensorsAPIHandler(ssApi), rtcHandler(rtc), gpsHandler(gps), sensorValueSize(sensorValueSize)
{
    weatherStation = { macAddress, { 0, 0 }, sh.GetSensorDataArray(), SENSOR_AMOUNT };

    sensorValuesArray = new SensorValues[SENSOR_AMOUNT];
    for (int i = 0; i < SENSOR_AMOUNT; i++)
    {
        sensorValuesArray[i] = { macAddress, sh.GetSensorDataArray()[i].sensorType, new SensorValue[sensorValueSize], 0 };
    }
}

WeatherStationHandler::~WeatherStationHandler()
{
    for (int i = 0; i < SENSOR_AMOUNT; i++)
    {
        delete sensorValuesArray[i].sensorValues;
    }
    delete sensorValuesArray;
}

bool WeatherStationHandler::MakeHandshakeWithAPI()
{
    // Set last known GPS location to weatherStation struct
    weatherStation.gpsLocation = gpsHandler.ReadGpsLocationFromEEPROM();

    // Make handshake with API
    timeSlot = skySensorsAPIHandler.MakeHandshake(weatherStation);
    if (timeSlot.intervalSeconds == 0)
    {
        LogHandler.LogError(200, 5, "Failed to make handshake with API.");
        return false;
    }
    LogHandler.LogInfo("Handshake was made with API, received timeslot: " + String(timeSlot.secondToTriggerAt) + "/" + String(timeSlot.intervalSeconds));
    return true;
}

void WeatherStationHandler::TakeMeasurements()
{
    for (int i = 0; i < weatherStation.sensorAmount; i++)
    {
        SensorValues& sensorValues = sensorValuesArray[i];
        SensorValue& sensorValue = sensorValues.sensorValues[sensorValues.sensorValueAmount];
        sensorValue.unixTime = rtcHandler.GetDateTimeNow().unixtime();
        sensorValue.value = sensorHandler.ReadSensorValueBasedOnType(sensorValues.sensorType);

        Serial.print(", type: " + String((int)sensorValues.sensorType) + " and value: " + String(sensorValue.value));
        
        if (sensorValues.sensorValueAmount < sensorValueSize)
        {
            sensorValues.sensorValueAmount++;
        }
        else
        {
            LogHandler.LogError(500, 5, "Buffer overload, sensorValues has reached its limit!");
        }
    }
    Serial.println();
}

void WeatherStationHandler::SendMeasurementsToServer()
{
    for (int i = 0; i < weatherStation.sensorAmount; i++)
    {
        if (!skySensorsAPIHandler.SendMesurementsToServer(sensorValuesArray[i]))
        {
            LogHandler.LogError(50, 3, "Failed to send mesurements to server.");
            continue;
        }
        sensorValuesArray[i].sensorValueAmount = 0;
    }
}

TimeSlot& WeatherStationHandler::GetTimeSlot()
{
    return timeSlot;
}
