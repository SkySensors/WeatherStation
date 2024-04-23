#pragma once

class CountTrigger
{
private:
    bool isTriggeredInInteval = true;

public:
    // Returns true once when counter is more than triggerValue until the counter is less than the triggerValue
    bool ShouldTrigger(int counter, int triggerValue)
    {
        if (counter > triggerValue)
        {
            if (!isTriggeredInInteval)
            {
                isTriggeredInInteval = true;
                return true;
            }
        }
        else
        {
            isTriggeredInInteval = false;
        }
        return false;
    }
};
