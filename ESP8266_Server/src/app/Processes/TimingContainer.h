#ifndef TIMINGCONTAINER_H
#define TIMINGCONTAINER_H

#include "Timing.h"
#include "ProcessQueue.h"

class TimingContainer {
    Timing timings[NR_OF_TIMINGS];
    Timing timingsBuffer[NR_OF_TIMINGS];
    ProcessQueue* processQueue;

    void loadTimingsFromFlash();
public:
    void parseTimings(Shutter_Request&);

    void checkTimings(int, int, int); 
    void initialize(ProcessQueue*);
    void disableEarlierSettings();

    Timing& getTiming(int id) { return this->timings[id]; }
};

#endif