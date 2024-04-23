#include "network_handler.hpp"
#include "log_handler.hpp"
#ifdef ESP32
#include "WiFi.h"
#endif

NetworkHandler::~NetworkHandler()
{
    delete client;
}

bool NetworkHandler::InitializeEthernetSheild(String macAddress)
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

bool NetworkHandler::InitializeEsp32Wifi(const char *ssid, const char *password)
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

Client &NetworkHandler::GetClient()
{
    return *client;
}

String NetworkHandler::GetMacAddress()
{
    return macAddress;
}

void NetworkHandler::GetMacAddressAsBytes(byte macAddress[6])
{
    int i = 0;
    for (int j = 0; j < this->macAddress.length(); j++)
    {
        String number = this->macAddress[j + 0] + String(this->macAddress[j + 1]);
        macAddress[i] = (byte)strtol(number.c_str(), NULL, 16);
        i++;
        j += 2;
    }
    this->macAddress.substring(0, 2);
}

String NetworkHandler::GetIpAsString()
{
    return String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
}
