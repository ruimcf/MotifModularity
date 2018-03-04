#ifndef _FAILOBJECT_
#define _FAILOBJECT_

class FailObject:
{
  private:
    int totalTimesFailed;
    int consecutiveTimesFailed;
    int totalTimesSuccess;
    int consecutiveTimesSuccess;
    int totalTimes;

  public:
    FailObject();
    void recordFail();
    void recordSuccess();
    bool finished();
}

#endif