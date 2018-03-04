#include "FailObject.h"

FailObject::FailObject()
{
    totalTimes = 0;
    totalTimesFailed = 0;
    totalTimesSuccess = 0;
    consecutiveTimesFailed = 0;
    consecutiveTimesSuccess = 0;
}

void FailObject::recordFail()
{
    totalTimes++;
    totalTimesFailed++;
    consecutiveTimesFailed++;
    consecutiveTimesSuccess = 0;
}

void FailObject::recordSuccess()
{
    totalTimes++;
    totalTimesSuccess++;
    consecutiveTimesSuccess++;
    consecutiveTimesFailed = 0;
}

bool FailObject::finished()
{
    if (consecutiveTimesFailed > 100 && totalTimesFailed > 1000000)
    {
        return false;
    }
    else
    {
        return true;
    }
}