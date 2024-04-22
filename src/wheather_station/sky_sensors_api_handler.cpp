#include "sky_sensors_api_handler.hpp"
#include "log_handler.hpp"

SkySensorsAPIHandler::SkySensorsAPIHandler(Client &client, const char* baseUrl, int port)
    : client(client, baseUrl, port)
{
}

TimeSlot SkySensorsAPIHandler::MakeHandshake(WheatherStation &wheatherStation)
{
    String json = "";
    wheatherStation.ToJson(json);
    String response = SendPostRequest("/api/weatherstation/handshake", json);
    
    if (response.length() == 0)
    {
        return { 0, 0 };
    }

    TimeSlot timeSlot;
    timeSlot.FromJson(response);
    return timeSlot;
}

bool SkySensorsAPIHandler::SendMesurementsToServer(SensorValues& sensorValues)
{
    String json = "";
    sensorValues.ToJson(json);
    json = "[" + json + "]";
    return SendPostRequest("/api/weatherstation", json).length() != 0;
}

unsigned long SkySensorsAPIHandler::GetServerTimeInUnixFormat()
{
    String response = SendGetRequest("/api/time");
    return response.length() != 0 ? (unsigned long)response.substring(0, 10).toDouble() : -1;
}

String SkySensorsAPIHandler::SendGetRequest(const char* path)
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

String SkySensorsAPIHandler::SendPostRequest(const char* path, String jsonBody)
{
    const char* contentType = "application/json";
    client.post(path, contentType, jsonBody);
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();
    client.stop();
    
    if (statusCode < 200 || statusCode > 299)
    {
        LogHandler.LogError(0, 0, "POST request to " + String(path) + " failed with code " + String(statusCode));
        return "";
    }
    return response.length() != 0 ? response : "OK";
}
