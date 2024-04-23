#include "gps_handler.hpp"
#include "log_handler.hpp"
#include <EEPROM.h>

void GpsHandler::Initialize()
{
    Serial2.begin(9600);
#ifdef ESP32
    EEPROM.begin(64);
#endif
}

bool GpsHandler::AttemptSatelliteGpsCommunication()
{
    while (Serial2.available() > 0)
    {
        gps.encode(Serial2.read());
        if (gps.location.isUpdated())
        {
            GpsLocation gpsLocation = { gps.location.lat() + 0.01f, gps.location.lng() + 0.02f };
            LogHandler.LogInfo("GPS location found, latitude:" + String(gpsLocation.latitude) + " longitude:" + String(gpsLocation.longitude));
            
            SaveGpsLocationToEEPROM(gpsLocation);
            isNewGpsLocationFound = true;
            return true;
        }
    } 
    return false;
}

bool GpsHandler::IsGpsLocationFound()
{
    return isNewGpsLocationFound;
}

GpsLocation GpsHandler::ReadGpsLocationFromEEPROM()
{
    GpsLocation gpsLocation;
    uint8_t* gpsLocationBytes = reinterpret_cast<uint8_t*>(&gpsLocation);
    for (int i = 0; i < sizeof(GpsLocation); i++)
    {
        gpsLocationBytes[i] = EEPROM.read(i);
    }
    LogHandler.LogInfo("Read GPS location from EEPROM, latitude:" + String(gpsLocation.latitude) + " longitude:" + String(gpsLocation.longitude));
    
    if (isnan(gpsLocation.latitude) || isnan(gpsLocation.longitude))
    {
      return { 1, 1 };
    }
    return gpsLocation;
}

void GpsHandler::SaveGpsLocationToEEPROM(GpsLocation gpsLocation)
{
    uint8_t* gpsLocationBytes = reinterpret_cast<uint8_t*>(&gpsLocation);
    for (int i = 0; i < sizeof(GpsLocation); i++)
    {
        EEPROM.write(i, gpsLocationBytes[i]);
    }
#ifdef ESP32
    EEPROM.commit();
#endif
    LogHandler.LogInfo("Saved GPS location to EEPROM.");
    
    // Added a hard delay here for safety reason in case this function gets called in an infinity loop
    delay(5000);
}
