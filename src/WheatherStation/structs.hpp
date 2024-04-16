#define MAX_SENSORS 3

enum class SensorType
{
    Temperature = 0,
    Light = 1,
    Humidity = 2,
};

struct Sensor
{
    float currentValue;
    SensorType sensorType;
};

struct GpsLocation
{
    float longitude;
    float latitude;
};

class WheatherStation
{
public:
    char macAddress[15];
    int sensorAmount;
    GpsLocation gpsLocation;
    Sensor sensors[MAX_SENSORS];

public:
    WheatherStation(char macAddress[15], GpsLocation gpsLocation, Sensor* sensors, int sensorAmount = MAX_SENSORS)
        : gpsLocation(gpsLocation), sensorAmount(sensorAmount)
    {
        memcpy(this->macAddress, macAddress, 15);
        memcpy(this->sensors, sensors, sizeof(Sensor) * sensorAmount);
    }
};
