#ifndef TIMINGCONTAINER_H
#define TIMINGCONTAINER_H

#include "Timing.h"
#include "ProcessQueue.h"

class TimingContainer {
    Timing timings[NR_OF_TIMINGS];
    Timing timingsBuffer[NR_OF_TIMINGS];
    ProcessQueue* processQueue;

    void loadTimingsFromFlash();
    void createJsonObject(JsonObject&);
public:
    void readTimingsFromFlash(String& target);
    void saveTimingsToFlash(const String&);
    void parseTimings(JsonObject&);

    void checkTimings(int, int, int); 
    void initialize(ProcessQueue*);
    void disableEarlierSettings();
};

#endif