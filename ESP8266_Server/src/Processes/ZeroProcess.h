#ifndef ZEROPROCESS_H
#define ZEROPROCESS_H

#include "SettingProcess.h"

class ZeroProcess : public SettingProcess {
    bool zeroFound = false;
public:    
    void start(const float) override;
    bool checkFinished() override;
    std::tuple<Shutter_Event, int> generateMessage() override;
};

#endif