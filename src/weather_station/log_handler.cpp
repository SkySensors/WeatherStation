#include "log_handler.hpp"

void LogHandler::Initialize(byte initLedPin, byte errorLedPin)
{
    this->initLedPin = initLedPin;
    this->errorLedPin = errorLedPin;
    Serial.begin(9600);
    pinMode(initLedPin, OUTPUT);
    pinMode(errorLedPin, OUTPUT);
}

void LogHandler::LogError(int ledFreq, byte blinkCount, String message)
{
    Serial.println("[ERROR] " + message);
    for (byte i = 0; i < blinkCount; i++)
    {
        digitalWrite(errorLedPin, HIGH);
        delay(ledFreq);
        digitalWrite(errorLedPin, LOW);
        delay(ledFreq);
    }
    delay(ledFreq);
}

void LogHandler::LogInfo(String message)
{
    Serial.println("[INFO] " + message);
}

void LogHandler::SetInitLedStatus(bool status)
{
    digitalWrite(initLedPin, status);
}
