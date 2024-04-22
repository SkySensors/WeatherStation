#pragma once
#include "def.hpp"

class SkySensorsAPIHandler
{
private:
    HttpClient client;

public:
    SkySensorsAPIHandler(Client &client, const char* baseUrl, int port);

public:
    // Used to make handshake with API
    // Returns timeslot if successfull or { 0, 0 } when it fails
    TimeSlot MakeHandshake(WheatherStation& wheatherStation);

    // Used to send sensor value mesurements to API
    // Returns true if successfull
    bool SendMesurementsToServer(SensorValues& sensorValues);

    // Returns current time from server in unix format
    unsigned long GetServerTimeInUnixFormat();

private:
    // Used to send a GET request
    // Returns content or OK if successfull or "" on a failure
    String SendGetRequest(const char* path);

    // Used to send a POST request
    // Returns content or OK if successfull or "" on a failure
    String SendPostRequest(const char* path, String jsonBody);
};
