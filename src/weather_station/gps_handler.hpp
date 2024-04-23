#pragma once
#include "def.hpp"

class GpsHandler
{
private:
    TinyGPSPlus gps;
    bool isNewGpsLocationFound = false;

public:
    // Used to initialize the GpsHandler
    void Initialize()
    {
        Serial2.begin(9600);
#ifdef ESP32
        EEPROM.begin(64);
#endif
    }

    // Used to communicate with the satellites inorder to find the GPS location
    // When the GPS location is found the value will be saved to the EEPROM
    // Returns true if GPS location was found
    bool AttemptSatelliteGpsCommunication()
    {
        while (Serial2.available() > 0)
        {
            gps.encode(Serial2.read());
            if (gps.location.isUpdated())
            {
                GpsLocation gpsLocation = { gps.location.lat(), gps.location.lng() };
                LogHandler.LogInfo("GPS location found, latitude:" + String(gpsLocation.latitude) + " longitude:" + String(gpsLocation.longitude));
                
                SaveGpsLocationToEEPROM(gpsLocation);
                isNewGpsLocationFound = true;
                return true;
            }
        } 
        return false;
    }

    // Returns true when GPS location is found
    bool IsGpsLocationFound()
    {
        return isNewGpsLocationFound;
    }

public:
    // Reads GPS location from EEPROM
    GpsLocation ReadGpsLocationFromEEPROM()
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
        return { 0, 0 };
        }
        return gpsLocation;
    }

private:
    // Saves GPS location to EEPROM at address 0 to 8
    void SaveGpsLocationToEEPROM(GpsLocation gpsLocation)
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
};