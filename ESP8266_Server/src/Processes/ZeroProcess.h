#ifndef NULLPROCESS_H
#define NULLPROCESS_H

#include "SettingProcess.h"

enum ZeroState{
    up,
    down,
    find
};

class ZeroProcess : public SettingProcess {
public:
    static ZeroProcess zeroProcess;
    static void processNull(ZeroState);
    ZeroState zeroState;

    bool zeroFound = false;
    void start();
    bool checkFinished();
    void generateMessage();
};

#endif