#pragma once
#include "def.hpp"
#include "rtc_handler.hpp"

#define SENSOR_AMOUNT 3

class SensorHandler
{
private:
    byte lightSensorAnalogPin;
    DHT dhtSensor;
    SensorData sensorDataArray[SENSOR_AMOUNT];

public:
    SensorHandler(byte lightSensorAnalogPin, byte dhtSensorPin)
        : lightSensorAnalogPin(lightSensorAnalogPin), dhtSensor(dhtSensorPin, AM2301), 
            sensorDataArray({ {SensorType::Temperature}, { SensorType::Light}, { SensorType::Humidity} })
    {
    }

public:
    // Used to initialize SensorHandler
    void Initialize()
    {
        dhtSensor.begin(55);
    }

public:
    // Used to read a sensor value based on the type being passed
    // Returns a sensor value
    float ReadSensorValueBasedOnType(SensorType sensorType)
    {
        switch (sensorType)
        {
        case SensorType::Temperature:
            return ReadTemperatureSensor();
        case SensorType::Humidity:
            return ReadHumiditySensor();
        case SensorType::Light:
            return ReadLightSensor();
        }
        return -1;
    }

    // Used to read current light sensor value
    // Returns light sensor value in lumens
    float ReadLightSensor()
    {
        static int vin = 5;
        static float r = 10000;

        int vo = analogRead(lightSensorAnalogPin);

        // Conversion analog to voltage
        float vout = float(vo) * (vin / float(MAX_ANALOG_VALUE));

        // Conversion voltage to resistance
        float rldr = (r * (vin - vout))/vout; 

        // Conversion resitance to lumen
        int phys = 500 / (rldr / 1000); 
        return phys;
    }

    // Used to read current temperature sensor value
    // Returns temperatur in celsius
    float ReadTemperatureSensor()
    {
        float value = dhtSensor.readTemperature(false, false);
        if (isnan(value))
        {
            LogHandler.LogError(100, 1, "Reading NAN from DHT sensor!");
        }
        return value;
    }

    // Used to read current humidity sensor value
    // Returns humidity in procent
    float ReadHumiditySensor()
    {
        float value = dhtSensor.readHumidity(false);
        if (isnan(value))
        {
            LogHandler.LogError(100, 1, "Reading NAN from DHT sensor!");
        }
        return value;
    }

public:
    // Returns an array of data about each sensor
    SensorData* GetSensorDataArray()
    {
        return sensorDataArray;
    }
};
