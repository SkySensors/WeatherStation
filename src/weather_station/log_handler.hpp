#pragma once
#include "def.hpp"

inline class LogHandler
{
private:
    byte initLedPin = 0;
    byte errorLedPin = 0;

public:
    // Used to initialize the log handler
    void Initialize(byte initLedPin, byte errorLedPin);

    // Used to log error via serial and set a error blink pattern
    void LogError(int ledFreq, byte blinkCount, String message);

    // Used to log information via serial
    void LogInfo(String message);

public:
    // Used to set status led either to HIGH or LOW
    void SetInitLedStatus(bool status);
}LogHandler;
