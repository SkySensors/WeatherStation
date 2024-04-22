#pragma once
#include "def.hpp"
#define SENSOR_AMOUNT 3

class SensorHandler
{
private:
    byte lightSensorAnalogPin;
    DHT dhtSensor;
    SensorData sensorDataArray[SENSOR_AMOUNT];

public:
    SensorHandler(byte lightSensorAnalogPin, byte dhtSensorPin);

public:
    // Used to initialize SensorHandler
    void Initialize();

public:
    // Used to read a sensor value based on the type being passed
    // Returns a sensor value
    float ReadSensorValueBasedOnType(SensorType sensorType);

    // Used to read current light sensor value
    // Returns light sensor value in lumens
    float ReadLightSensor();

    // Used to read current temperatur sensor value
    // Returns temperatur in celsius
    float ReadTemperaturSensor();

    // Used to read current humidity sensor value
    // Returns humidity in procent
    float ReadHumiditySensor();

public:
    // Returns an array of data about each sensor
    SensorData* GetSensorDataArray();
};
