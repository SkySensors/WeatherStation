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
        HttpResponse httpResponse = SendPostRequest("/api/weatherstation/handshake", json);
        
        if (!httpResponse.IsSuccess())
        {
            return { 0, 0 };
        }

        TimeSlot timeSlot;
        timeSlot.FromJson(httpResponse.content);
        return timeSlot;
    }

    // Used to send sensor value mesurements to API
    // Returns HttpContent
    HttpResponse SendMesurementsToServer(SensorValues& sensorValues)
    {
        String json = "";
        sensorValues.ToJson(json);
        json = "[" + json + "]";
        return SendPostRequest("/api/weatherstation", json);
    }

    // Returns current time from server in unix format
    unsigned long GetServerTimeInUnixFormat()
    {
        HttpResponse httpResponse = SendGetRequest("/api/time");
        char* end;
        return httpResponse.IsSuccess() ? strtol(httpResponse.content.substring(0, 10).c_str(), &end, 0) : -1;
    }

private:
    // Used to send a GET request
    // Returns content or OK if successfull or "" on a failure
    HttpResponse SendGetRequest(const char* path)
    {
        client.get(path);
        HttpResponse httpResponse = { client.responseStatusCode(), client.responseBody() };
        client.stop();
        
        if (!httpResponse.IsSuccess())
        {
            LogHandler.LogError(0, 0, "GET request to " + String(path) + " failed with code " + String(httpResponse.status));
        }
        return httpResponse;
    }

    // Used to send a POST request
    // Returns content or OK if successfull or "" on a failure
    HttpResponse SendPostRequest(const char* path, String jsonBody)
    {
        const char* contentType = "application/json";
        client.post(path, contentType, jsonBody);
        HttpResponse httpResponse = { client.responseStatusCode(), client.responseBody() };
        client.stop();
        if (!httpResponse.IsSuccess())
        {
            LogHandler.LogError(0, 0, "POST request to " + String(path) + " failed with code " + String(httpResponse.status) + " json: \n" + String(jsonBody));
        }
        return httpResponse;
    }
};
