#include "Libraries/dht/DHT_U.h"
#include "Libraries/dht/DHT.h"

DHT dht(7, DHT11);

float AnalogReadTemperaturCelsius(int analogPin)
{
    dht.begin(55);
    static float R1 = 10000;
    static float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

    int Vo = analogRead(analogPin);
    float R2 = R1 * (1023.0 / (float)Vo - 1.0);
    float logR2 = log(R2);
    float T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));

    return T - 273.15f;
}

float AnalogReadLightLumen(int analogPin)
{
    static int VIN = 5;
    static float R = 10000;

    int Vo = analogRead(analogPin);
    float Vout = float(Vo) * (VIN / float(1023));// Conversion analog to voltage
    float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
    int phys = 500 / (RLDR / 1000); // Conversion resitance to lumen
    return phys;
}

float ReadHumidity()
{
    return dht.readHumidity(false);
}

float ReadTemp()
{
    return dht.readTemperature(false, false);
}

void setup() 
{
    Serial.begin(9600);
}

void loop() 
{
    Serial.print("Temperature 1: "); 
    Serial.print(AnalogReadTemperaturCelsius(A0));
    Serial.print(" C");   

    Serial.print(", Light 1: "); 
    Serial.print(AnalogReadLightLumen(A1));
    Serial.print(" lumen");   

    Serial.print(", Temp: "); 
    Serial.print(ReadTemp());
    Serial.println("C");   

    Serial.print(", Humidity: "); 
    Serial.print(ReadHumidity());
    Serial.println("%");   

    delay(2000);
}
