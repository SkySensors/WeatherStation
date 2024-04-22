#include "count_trigger.hpp"
#include "log_handler.hpp"

bool CountTrigger::ShouldTrigger(int counter, int triggerValue)
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