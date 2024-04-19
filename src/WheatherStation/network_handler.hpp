#pragma once
#include "def.hpp"

static class DHCPHandler
{
public:
    IPAddress GetIP(byte* mac);
}DHCPHandler;