#include "network_handler.hpp"
#include "sky_sensors_api_handler.hpp"
#include "sensor_handler.hpp"
#include "rtc_handler.hpp"
#include "log_handler.hpp"
#include "weather_station_handler.hpp"
#include "count_trigger.hpp"
#include "gps_handler.hpp"

#define SENSOR_VALUE_BUFFER_SIZE 60
#define SKY_SENSORS_API_URL "skyapi.viften.elkok.dk"
#define SKY_SENSORS_API_PORT 888

#ifdef ESP32
#define WIFI_SSID "NETGEAR"
#define WIFI_PASSWORD "Jonas2208"
#define LIGHT_SENSOR_ANALOG_PIN 36
#define DHT_SENSOR_PIN 27
#define INIT_LED_PIN 32
#define ERROR_LED_PIN 33
#else
#define ETHERNET_SHEILD_MAC "90:A2:DA:10:55:88"
#define LIGHT_SENSOR_ANALOG_PIN A0
#define DHT_SENSOR_PIN 7
#define INIT_LED_PIN 13
#define ERROR_LED_PIN 12
#endif

NetworkHandler* networkHandler;
SkySensorsAPIHandler* skySensorsAPIHandler;
SensorHandler* sensorHandler;
RTCHandler* rtcHandler;
WeatherStationHandler* weatherStationHandler;
GpsHandler* gpsHandler;
CountTrigger rtcSyncTrigger;
CountTrigger sendMeasurementsTrigger;
CountTrigger takeMeasurementsTrigger;

void setup()
{
    // Initialize LogHandler
    LogHandler.Initialize(INIT_LED_PIN, ERROR_LED_PIN);
    LogHandler.LogInfo("Starting...");
    
    // Set status led to on to indicate that the weather station is being initialized
    LogHandler.SetInitLedStatus(HIGH); 
    
    // Initializes the network modules
    networkHandler = new NetworkHandler();
#ifdef ESP32
    while(!networkHandler->InitializeEsp32Wifi(WIFI_SSID, WIFI_PASSWORD)) 
#else
    while(!networkHandler->InitializeEthernetSheild(ETHERNET_SHEILD_MAC))
#endif
    { 
        delay(1000); 
    }

    // Initializes the SkySensorsAPIHandler
    skySensorsAPIHandler = new SkySensorsAPIHandler(networkHandler->GetClient(), SKY_SENSORS_API_URL, SKY_SENSORS_API_PORT);

    // Initializes weather station modules
    sensorHandler = new SensorHandler(LIGHT_SENSOR_ANALOG_PIN, DHT_SENSOR_PIN);
    sensorHandler->Initialize();

    rtcHandler = new RTCHandler(*skySensorsAPIHandler);
    rtcHandler->Initialize();

    gpsHandler = new GpsHandler();
    gpsHandler->Initialize();

    weatherStationHandler = new WeatherStationHandler(
        *sensorHandler, *skySensorsAPIHandler, *rtcHandler, 
        *gpsHandler, networkHandler->GetMacAddress(), SENSOR_VALUE_BUFFER_SIZE);

    // Makes handshake with the API
    while(!weatherStationHandler->MakeHandshakeWithAPI())
    {
        delay(1000);
    }

    // The status led turns off when the weather station is successfully initialized
    LogHandler.SetInitLedStatus(LOW);
}

void loop() 
{   
    int hourSeconds = rtcHandler->GetCurrentHourInSeconds();

    // Synchronizes the RTC every 30 minute
    if (rtcSyncTrigger.ShouldTrigger(hourSeconds, 1800))
    {
        rtcHandler->SynchronizeWithServerTime();
    }

    // Takes measurements every two second
    if (takeMeasurementsTrigger.ShouldTrigger(hourSeconds % 2, 0))
    {
        weatherStationHandler->TakeMeasurements();
    }

    // Sends mesurements to server when time slot inteval is triggered 
    TimeSlot& timeSlot = weatherStationHandler->GetTimeSlot();
    if (sendMeasurementsTrigger.ShouldTrigger(hourSeconds % timeSlot.intervalSeconds, timeSlot.secondToTriggerAt))
    {
        weatherStationHandler->SendMeasurementsToServer();
    }

    // If no GPS location is found, then try to get the gps location from the satellites
    if (!gpsHandler->IsGpsLocationFound() && gpsHandler->AttemptSatelliteGpsCommunication())
    {
        // When GPS location is found, then make one last handshake with the API to notify about the new GPS location
        while(!weatherStationHandler->MakeHandshakeWithAPI())
        {
            delay(1000);
        }
    }

    delay(250);
}
