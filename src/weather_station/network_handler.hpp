#pragma once
#include "def.hpp"

class NetworkHandler
{
private:
    Client* client = nullptr;
    IPAddress ip;
    String macAddress = "";

public:
    ~NetworkHandler();

public:
    // Used initialize NetworkHandler using Ethernet Sheild
    // Returns true when initialized successfully
    bool InitializeEthernetSheild(String macAddress);

    // Used initialize NetworkHandler using ESP32 WiFi
    // Returns true when initialized successfully
    bool InitializeEsp32Wifi(const char* ssid, const char* password);

public:
    // Returns generic network client
    Client& GetClient();

    // Returns mac address as String
    String GetMacAddress();

    // Returns mac address in bytes into parsed array
    void GetMacAddressAsBytes(byte macAddress[6]);

private:
    // Returns IP in string format
    String GetIpAsString();
};