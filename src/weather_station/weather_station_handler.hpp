#pragma once
#include "def.hpp"
#include "sky_sensors_api_handler.hpp"
#include "sensor_handler.hpp"
#include "rtc_handler.hpp"
#include "gps_handler.hpp"

class WeatherStationHandler
{
private:
    SensorHandler& sensorHandler;
    SkySensorsAPIHandler& skySensorsAPIHandler;
    RtcHandler& rtcHandler;
    GpsHandler& gpsHandler;

private:
    WeatherStation weatherStation;
    TimeSlot timeSlot;
    SensorValues* sensorValuesArray;
    int sensorValueSize = 0;

public:
    WeatherStationHandler(SensorHandler& sensorHandler, SkySensorsAPIHandler& skySensorsAPIHandler, 
        RtcHandler& rtcHandler, GpsHandler& gpsHandler, String macAddress, int sensorValueSize)
        : sensorHandler(sensorHandler), skySensorsAPIHandler(skySensorsAPIHandler), 
        rtcHandler(rtcHandler), gpsHandler(gpsHandler), sensorValueSize(sensorValueSize)
    {
        weatherStation = { macAddress, { 0, 0 }, sensorHandler.GetSensorDataArray(), SENSOR_AMOUNT };

        sensorValuesArray = new SensorValues[SENSOR_AMOUNT];
        for (int i = 0; i < SENSOR_AMOUNT; i++)
        {
            sensorValuesArray[i] = { macAddress, sensorHandler.GetSensorDataArray()[i].sensorType, new SensorValue[sensorValueSize], 0 };
        }
    }
    ~WeatherStationHandler()
    {
        for (int i = 0; i < SENSOR_AMOUNT; i++)
        {
            delete sensorValuesArray[i].sensorValues;
        }
        delete sensorValuesArray;
    }

public:
    // Used to make handshake with API
    // Returns true if successful
    bool MakeHandshakeWithAPI()
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

    // Used to take measurements of each sensor and store the values in a buffer
    void TakeMeasurements()
    {
        for (int i = 0; i < weatherStation.sensorAmount; i++)
        {
            SensorValues& sensorValues = sensorValuesArray[i];
            SensorValue& sensorValue = sensorValues.sensorValues[sensorValues.sensorValueAmount];
            sensorValue.unixTime = rtcHandler.GetDateTimeNow().unixtime();
            sensorValue.value = sensorHandler.ReadSensorValueBasedOnType(sensorValues.sensorType);

            if (sensorValues.sensorValueAmount < sensorValueSize)
            {
                sensorValues.sensorValueAmount++;
            }
            else
            {
                LogHandler.LogError(500, 5, "Buffer overload, sensorValues has reached its limit!");
            }
        }
    }

    // Used to send all the sensor measurements to the API and clear the buffer
    void SendMeasurementsToServer()
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

public:
    // Returns time slot
    TimeSlot& GetTimeSlot()
    {
        return timeSlot;
    }
};