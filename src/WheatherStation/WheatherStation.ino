#include <TinyGPS++.h>

#include <DHT_U.h>
#include <DHT.h>

#include <Wire.h>
#include <DS3231.h>

#include "structs.hpp"
#include "dhcp_handler.hpp"
#include "sky_sensors_api.hpp"

#define SensorValueBufferSize 80

char serverAddress[] = "skyapi.viften.elkok.dk"; 
char macAddressStr[18] = "90:A2:DA:10:55:88";
int port = 888;

byte macAdd[] = 
{
  0x90, 0xA2, 0xDA, 0x10, 0x55, 0x88
};

EthernetClient eclient;
HttpClient client = HttpClient(eclient, serverAddress, port);
SkySensorsAPI skySensorsAPI(client);
TinyGPSPlus gps;
DHT dht(7, AM2301);
RTClib myRTC;
DS3231 Clock;

TimeSlot timeSlot;
Sensor sensors[3] = { {SensorType::Temperature}, { SensorType::Light}, { SensorType::Humidity} };
WheatherStation wheatherStation(macAddressStr, { 20, 10 }, sensors, 3);
int sensorValueIndex = 0;

void setup() 
{
    Serial.begin(9600);
    Serial2.begin(9600);

    dht.begin(55);
    IPAddress ip = NetworkHandler.GetDhcpIP(macAdd);
    Serial.print("IP Address is: ");
    Serial.println(ip);

    Wire.begin();
    unsigned long unixTime = skySensorsAPI.GetCurrentUnixTime();
    Clock.setEpoch({unixTime}, true);
    Serial.println("RTC clock has been sync with server time: " + String(unixTime));

    timeSlot = skySensorsAPI.MakeWeatherStationHandshake(wheatherStation);
    Serial.println("Time slot is: " + String(timeSlot.secondNumber) + ":" + String(timeSlot.intervalSeconds));
}

void loop() 
{    
    DateTime now = myRTC.now();
    for (int i = 0; i < wheatherStation.sensorAmount; i++)
    {
        Sensor& sensor = wheatherStation.sensors[i];
        SensorValue& sensorValue = sensor.sensorValues[sensorValueIndex];
        sensorValue.unixTime = now.unixtime();
        sensor.sensorValueAmount = sensorValueIndex;
        
        switch (sensor.sensorType)
        {
        case SensorType::Humidity:
            sensorValue.value = ReadHumidity();
            break;
        case SensorType::Temperature:
            sensorValue.value = ReadTemp();
            break;
        case SensorType::Light:
            sensorValue.value = AnalogReadLightLumen(A0);
            break;
        }
        //Serial.println("Sensor mesuerment: " + String(sensorValue.value) + " at " + String(sensorValue.unixTime) + " index " + String(sensorValueIndex) + " for type" + String((int)sensor.sensorType));
    }
    if (sensorValueIndex < MAX_SENSOR_VALUE_BUFFER_SIZE)
    {
        sensorValueIndex++;
    }
    else
    {
        Serial.println("Buffer overload, sensorValueIndex reached its limit!");
    }

    PrintTime();

    delay(1000);
    // while (Serial2.available() > 0)
    // {
    //     gps.encode(Serial2.read());
    //     Serial.println("S2 was available.");

    //     if (gps.location.isUpdated())
    //     {
    //         Serial.print("Latitude= "); 
    //         Serial.print(gps.location.lat(), 6);
    //         Serial.print(" Longitude= "); 
    //         Serial.println(gps.location.lng(), 6);
    //     }
    // }
}

bool isTriggeredInInteval = true;
bool isTriggeredInInteval2 = true;
void PrintTime()
{
    DateTime now = myRTC.now();
    
    int hourSecond = now.minute() * 60 + now.second();
    int counter = hourSecond % timeSlot.intervalSeconds;

    if (hourSecond > 1800)
    {
        if (!isTriggeredInInteval2)
        {
            unsigned long unixTime = skySensorsAPI.GetCurrentUnixTime();
            Clock.setEpoch({unixTime}, true);
            Serial.println("RTC clock has been sync with server time: " + String(unixTime));
        }
        isTriggeredInInteval2 = true;
    }
    else
    {
        isTriggeredInInteval2 = false;
    }

    if (counter > timeSlot.secondNumber)
    {
        if (!isTriggeredInInteval)
        {
            for (int i = 0; i < wheatherStation.sensorAmount; i++)
            {
                skySensorsAPI.AddSensorValues(macAddressStr, wheatherStation.sensors[i]);
            }
            //Serial.println("Triggered");
            Serial.println("Time of RTC: " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + ":" + " - counter: " + String(counter));
            sensorValueIndex = 0;
        }
        isTriggeredInInteval = true;
    }
    else
    {
        isTriggeredInInteval = false;
    }
    //Serial.println("Time of RTC: " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + ":" + " - counter: " + String(counter));
}

float AnalogReadLightLumen(int analogPin)
{
    static int VIN = 5;
    static float R = 10000;

    int Vo = analogRead(analogPin);
    float Vout = float(Vo) * (VIN / float(1023));// Conversion analog to voltage
    float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
    int phys = 500 / (RLDR / 1000); // Conversion resitance to lumen
    return phys;
}

float ReadHumidity()
{
    return dht.readHumidity(false);
}

float ReadTemp()
{
    return dht.readTemperature(false, false);
}
