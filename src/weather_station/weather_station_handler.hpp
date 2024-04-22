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
    RTCHandler& rtcHandler;
    GpsHandler& gpsHandler;

private:
    WeatherStation weatherStation;
    TimeSlot timeSlot;
    SensorValues* sensorValuesArray;
    int sensorValueSize = 0;

public:
    WeatherStationHandler(SensorHandler& sensorHandler, SkySensorsAPIHandler& skySensorsAPIHandler, 
        RTCHandler& rtcHandler, GpsHandler& gpsHandler, String macAddress, int sensorValueSize);
    ~WeatherStationHandler();

public:
    // Used to make handshake with API
    // Returns true if successful
    bool MakeHandshakeWithAPI();

    // Used to take measurements of each sensor and store the values in a buffer
    void TakeMeasurements();

    // Used to send all the sensor measurements to the API and clear the buffer
    void SendMeasurementsToServer();

public:
    // Returns time slot
    TimeSlot& GetTimeSlot();
};