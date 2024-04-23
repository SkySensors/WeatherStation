#pragma once
#include "def.hpp"

class NetworkHandler
{
private:
    Client* client = nullptr;
    IPAddress ip;
    String macAddress = "";

public:
    ~NetworkHandler()
    {
        delete client;
    }

public:
    // Used initialize NetworkHandler using Ethernet Sheild
    // Returns true when initialized successfully
    bool InitializeEthernetSheild(String macAddress)
    {
        this->macAddress = macAddress;

        byte macAddressBytes[6] = { 0 };
        GetMacAddressAsBytes(macAddressBytes);

        if (Ethernet.begin(macAddressBytes) == 0) 
        {
            LogHandler.LogError(1000, 1, "Failed to initialize ethernet sheild.");
            return false;
        }

        byte dhcpState = Ethernet.maintain();
        if (Ethernet.maintain() != 0)
        {
            LogHandler.LogError(1000, 2, "Failed to initialize ethernet sheild due to a DHCP issue, dhcp error code is " + dhcpState);
            return false;
        }

        ip = Ethernet.localIP();
        LogHandler.LogInfo("Ethernet sheild has been initialized, ip is " + GetIpAsString());
        
        if (client == nullptr)
        {
            client = new EthernetClient();
        }
        return true;
    }

    // Used initialize NetworkHandler using ESP32 WiFi
    // Returns true when initialized successfully
    bool InitializeEsp32Wifi(const char* ssid, const char* password)
    {
#ifdef ESP32
        if (WiFi.begin(ssid, password) == 0)
        {
            LogHandler.LogError(1000, 1, "Failed to initialize ESP32 WiFi .");
            return false;
        }
        for (int i = 0; WiFi.status() != WL_CONNECTED; i++)
        {
            if (i >= 10)
            {
                LogHandler.LogError(1000, 2, "Failed to connect to ESP32 WiFi.");
                return false;
            }
            delay(500);
        }

        macAddress = WiFi.macAddress();
        ip = WiFi.localIP();
        LogHandler.LogInfo("ESP32 WiFi has been initialized, ip is " + GetIpAsString());

        if (client == nullptr)
        {
            client = new WiFiClient();
        }
        return true;
#else
        LogHandler.LogError(1000, 1, "ESP32 is not defined.");
        return false;
#endif
    }

public:
    // Returns generic network client
    Client& GetClient()
    {
        return *client;
    }

    // Returns mac address as String
    String GetMacAddress()
    {
        return macAddress;
    }


private:
    // Returns mac address in bytes into parsed array
    void GetMacAddressAsBytes(byte macAddress[6])
    {
        int i = 0;
        for (int j = 0; j < this->macAddress.length(); j++)
        {
            String number = this->macAddress[j + 0] + String(this->macAddress[j + 1]);
            macAddress[i] = (byte)strtol(number.c_str(), NULL, 16);
            i++;
            j += 2;
        }
    }

    // Returns IP in string format
    String GetIpAsString()
    {
        return String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
    }
};