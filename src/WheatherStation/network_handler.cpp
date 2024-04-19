#include "dhcp_handler.hpp"

IPAddress NetworkHandler::GetIP(byte* mac)
{
    if (Ethernet.begin(mac) == 0) 
    {
        Serial.println("Failed to configure Ethernet");
        return IPAddress(169, 254, 1, 2);
    }
    Ethernet.maintain();
    return Ethernet.localIP();
}