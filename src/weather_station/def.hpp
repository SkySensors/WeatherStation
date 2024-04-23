#pragma once
//#define ESP32

#ifdef ESP32
#define MAX_ANALOG_VALUE 4096
#else
#define MAX_ANALOG_VALUE 1024
#endif

#include <SPI.h>
#include <IPAddress.h>
#include <Wire.h>
#include <Ethernet2.h>
#include <ArduinoHttpClient.h>
#include <DHT_U.h>
#include <DHT.h>
#include <DS3231.h>
#include <TinyGPSPlus.h>
#include <EEPROM.h>
#ifdef ESP32
#include "WiFi.h"
#endif

#include "log_handler.hpp"
#include "structures.hpp"