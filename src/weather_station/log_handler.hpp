#pragma once

inline class LogHandler
{
private:
    byte statusLedPin = 0;
    byte errorLedPin = 0;

public:
    // Used to initialize the log handler
    void Initialize(byte statusLedPin, byte errorLedPin)
    {
        this->statusLedPin = statusLedPin;
        this->errorLedPin = errorLedPin;
        Serial.begin(19200);
        pinMode(statusLedPin, OUTPUT);
        pinMode(errorLedPin, OUTPUT);
    }

    // Used to log error via serial and set a error blink pattern
    void LogError(int ledFreq, byte blinkCount, String message)
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

    // Used to log information via serial
    void LogInfo(String message)
    {
        Serial.println("[INFO] " + message);
    }

public:
    // Used to set status led either to HIGH or LOW
    void SetStatusLed(bool status)
    {
        digitalWrite(statusLedPin, status);
    }
}LogHandler;
