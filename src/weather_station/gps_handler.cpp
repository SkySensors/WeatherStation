#include "gps_handler.hpp"
#include "log_handler.hpp"
#include <EEPROM.h>

GpsHandler::GpsHandler(byte rxSoftwarePin, byte txSoftwarePin)
    : softwareSerial(rxSoftwarePin, txSoftwarePin)
{
}

void GpsHandler::Initialize()
{
    softwareSerial.begin(9600);
}

bool GpsHandler::AttemptSatelliteGpsCommunication()
{
    while (softwareSerial.available() > 0)
    {
        gps.encode(softwareSerial.read());
        if (gps.location.isUpdated())
        {
            GpsLocation gpsLocation = { gps.location.lat(), gps.location.lng() };
            SaveGpsLocationToEEPROM(gpsLocation);
            isNewGpsLocationFound = true;

            LogHandler.LogInfo("GPS location found, latitude:" + String(gpsLocation.latitude) + " longitude:" + String(gpsLocation.longitude));
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
    GpsLocation gpsLocation = { 0, 0 };
    uint8_t* gpsLocationBytes = reinterpret_cast<uint8_t*>(&gpsLocation);
    for (int i = 0; i < sizeof(GpsLocation); i++)
    {
        gpsLocationBytes[i] = EEPROM.read(i);
    }
    LogHandler.LogInfo("Read GPS location from EEPROM, latitude:" + String(gpsLocation.latitude) + " longitude:" + String(gpsLocation.longitude));
    return gpsLocation;
}

void GpsHandler::SaveGpsLocationToEEPROM(GpsLocation gpsLocation)
{
    uint8_t* gpsLocationBytes = reinterpret_cast<uint8_t*>(&gpsLocation);
    for (int i = 0; i < sizeof(GpsLocation); i++)
    {
        EEPROM.write(i, gpsLocationBytes[i]);
    }
    LogHandler.LogInfo("Saved GPS location to EEPROM.");

    // Added a hard delay here for safety reason in case this function gets called in an infinity loop
    delay(5000);
}
