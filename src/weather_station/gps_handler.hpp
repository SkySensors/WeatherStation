#pragma once
#include "def.hpp"

class GpsHandler
{
private:
    TinyGPSPlus gps;
    SoftwareSerial softwareSerial;
    bool isNewGpsLocationFound = false;

public:
    GpsHandler(byte rxSoftwarePin, byte txSoftwarePin);

public:
    // Used to initialize the GpsHandler
    void Initialize();

    // Used to communicate with the satellites inorder to find the GPS location
    // When the GPS location is found the value will be saved to the EEPROM
    // Returns true if GPS location was found
    bool AttemptSatelliteGpsCommunication();

    // Returns true when GPS location is found
    bool IsGpsLocationFound();

public:
    // Reads GPS location from EEPROM
    GpsLocation ReadGpsLocationFromEEPROM();

private:
    // Saves GPS location to EEPROM at address 0 to 8
    void SaveGpsLocationToEEPROM(GpsLocation gpsLocation);
};