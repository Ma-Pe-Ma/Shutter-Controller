#ifndef NULLPROCESS_H
#define NULLPROCESS_H

#include "SettingProcess.h"

enum NullState{
    up,
    down,
    find
};

class NullProcess : public SettingProcess {
public:
    static NullProcess nullProcess;
    static void processNull(NullState);

    bool zeroFound = false;
    void start();
    bool checkFinished();
    void generateMessage();
};

#endif