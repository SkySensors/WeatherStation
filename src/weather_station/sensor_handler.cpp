#include "sensor_handler.hpp"
#include "rtc_handler.hpp"
#include "log_handler.hpp"

SensorHandler::SensorHandler(byte lightSensorAnalogPin, byte dhtSensorPin)
    : lightSensorAnalogPin(lightSensorAnalogPin), dhtSensor(dhtSensorPin, AM2301), 
        sensorDataArray({ {SensorType::Temperature}, { SensorType::Light}, { SensorType::Humidity} })
{
}

void SensorHandler::Initialize()
{
    dhtSensor.begin(55);
}

float SensorHandler::ReadSensorValueBasedOnType(SensorType sensorType)
{
    switch (sensorType)
    {
    case SensorType::Temperature:
        return ReadTemperaturSensor();
    case SensorType::Humidity:
        return ReadHumiditySensor();
    case SensorType::Light:
        return ReadLightSensor();
    }
    return -1;
}

float SensorHandler::ReadLightSensor()
{
    static int vin = 5;
    static float r = 10000;

    int vo = analogRead(lightSensorAnalogPin);

    // Conversion analog to voltage
    float vout = float(vo) * (vin / float(1023));

    // Conversion voltage to resistance
    float rldr = (r * (vin - vout))/vout; 

    // Conversion resitance to lumen
    int phys = 500 / (rldr / 1000); 
    return phys;
}

float SensorHandler::ReadTemperaturSensor()
{
    return dhtSensor.readTemperature(false, false);
}

float SensorHandler::ReadHumiditySensor()
{
    return dhtSensor.readHumidity(false);
}

SensorData *SensorHandler::GetSensorDataArray()
{
    return sensorDataArray;
}
