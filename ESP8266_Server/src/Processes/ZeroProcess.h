#ifndef ZEROPROCESS_H
#define ZEROPROCESS_H

#include "SettingProcess.h"

enum ZeroState{
    up,
    down,
    find
};

class ZeroProcess : public SettingProcess {
    bool zeroFound = false;

public:    
    void start(const float) override;
    bool checkFinished() override;
    RawMessage generateMessage() override;
};

#endif