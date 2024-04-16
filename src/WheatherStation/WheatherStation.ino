#include <Base64.h>
#include <TinyGPS++.h>

#include "Libraries/dht/DHT_U.h"
#include "Libraries/dht/DHT.h"
#include "structs.hpp"

TinyGPSPlus gps;
DHT dht(7, AM2301);

float AnalogReadTemperaturCelsius(int analogPin)
{
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
    Serial3.begin(9600);
    dht.begin(55);
    // Serial.begin(9600);
    // Serial3.begin(115200);
}

void loop() 
{    
    //Serial.read();
    // Serial.print("Temperature 1: "); 
    // Serial.print(AnalogReadTemperaturCelsius(A0));
    // Serial.print(" C");   

    // Serial.print(", Light 1: "); 
    // Serial.print(AnalogReadLightLumen(A1));
    // Serial.print(" lumen");   

    // Serial.print(", Temp: "); 
    // Serial.print(ReadTemp());
    // Serial.println("C");   

    // Serial.print(", Humidity: "); 
    // Serial.print(ReadHumidity());
    // Serial.println("%");   

    // Sensor sensors[3] = { {ReadTemp(), SensorType::Temperature}, {AnalogReadLightLumen(A1), SensorType::Light}, {ReadHumidity(), SensorType::Humidity} };
    // WheatherStation wheatherStation("00:00:00:00:00", { 20, 10 }, sensors, 3);

    while (Serial3.available() > 0)
    {
        gps.encode(Serial3.read());
        if (gps.location.isUpdated())
        {
            Serial.print("Latitude= "); 
            Serial.print(gps.location.lat(), 6);
            Serial.print(" Longitude= "); 
            Serial.println(gps.location.lng(), 6);
        }
    }

    // char* data = reinterpret_cast<char*>(&wheatherStation);

    // int inputStringLength = sizeof(WheatherStation);
    // int encodedLength = Base64.encodedLength(inputStringLength);
    // char encodedString[encodedLength + 1];
    // Base64.encode(encodedString, data, inputStringLength);

    // Serial3.println(encodedString);
}

// void serialEvent3() {
//   int index = 0;

//   while (Serial3.available()) {
//     index++;
//     Serial3.write("Hello world")

//     char inChar = Serial3.read();
//     Serial.write(inChar);
//     inString += inChar;
//     if (inChar == ']') {
//       if (inString.indexOf("[ON]")>0) {
//         digitalWrite(PIN_LED, HIGH);
//       }
//       else if (inString.indexOf("[OFF]")>0) {
//         digitalWrite(PIN_LED, LOW);
//       }
//       else
//       {
//         Serial.println("Wrong command");
//       }
//       inString = "";
//     }
//   }
// }
