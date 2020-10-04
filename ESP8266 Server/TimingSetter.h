#ifndef TIMINGSETTER_H
#define TIMINGSETTER_H

#include "Timing.h"
#include "SettingProcess.h"
#include <ArduinoJson.h>

class TimingSetter : public SettingProcess {
    Timing buffer[NR_OF_TIMINGS];

public:
    TimingSetter() {}
    static TimingSetter timingSetter;
    bool queued = false;
    void start();
    bool checkFinished();
    void processMessage(JsonObject&);
    void generateMessage();
    
};

#endif