#ifndef _FAILOBJECT_
#define _FAILOBJECT_

class FailObject
{
  public:
    FailObject();
    void recordFail();
    void recordSuccess();
    bool finished();

  protected:
    int totalTimesFailed;
    int consecutiveTimesFailed;
    int totalTimesSuccess;
    int consecutiveTimesSuccess;
    int totalTimes;
};

#endif