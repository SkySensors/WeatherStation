#pragma once
#include "def.hpp"

class SkySensorsAPIHandler
{
private:
    HttpClient client;

public:
    SkySensorsAPIHandler(Client &client, const char* baseUrl, int port)
        : client(client, baseUrl, port)
    {
    }

public:
    // Used to make handshake with API
    // Returns timeslot if successfull or { 0, 0 } when it fails
    TimeSlot MakeHandshake(WeatherStation& weatherStation)
    {
        String json = "";
        weatherStation.ToJson(json);
        String response = SendPostRequest("/api/weatherstation/handshake", json);
        
        if (response.length() == 0)
        {
            return { 0, 0 };
        }

        TimeSlot timeSlot;
        timeSlot.FromJson(response);
        return timeSlot;
    }

    // Used to send sensor value mesurements to API
    // Returns true if successfull
    bool SendMesurementsToServer(SensorValues& sensorValues)
    {
        String json = "";
        sensorValues.ToJson(json);
        json = "[" + json + "]";
        return SendPostRequest("/api/weatherstation", json).length() != 0;
    }

    // Returns current time from server in unix format
    unsigned long GetServerTimeInUnixFormat()
    {
        String response = SendGetRequest("/api/time");
        return response.length() != 0 ? (unsigned long)response.substring(0, 10).toDouble() : -1;
    }

private:
    // Used to send a GET request
    // Returns content or OK if successfull or "" on a failure
    String SendGetRequest(const char* path)
    {
        client.get(path);
        int statusCode = client.responseStatusCode();
        String response = client.responseBody();
        client.stop();
        
        if (statusCode < 200 || statusCode > 299)
        {
            LogHandler.LogError(0, 0, "GET request to " + String(path) + " failed with code " + String(statusCode));
            return "";
        }
        return response.length() != 0 ? response : "OK";
    }

    // Used to send a POST request
    // Returns content or OK if successfull or "" on a failure
    String SendPostRequest(const char* path, String jsonBody)
    {
        const char* contentType = "application/json";
        client.post(path, contentType, jsonBody);
        int statusCode = client.responseStatusCode();
        String response = client.responseBody();
        client.stop();
        
        if (statusCode < 200 || statusCode > 299)
        {
            LogHandler.LogError(0, 0, "POST request to " + String(path) + " failed with code " + String(statusCode) + " json: \n" + String(jsonBody));
            return "";
        }
        return response.length() != 0 ? response : "OK";
    }
};
