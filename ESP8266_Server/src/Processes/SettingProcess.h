#ifndef SETTINGPROCESS_H
#define SETTINGPROCESS_H

#include "../MessageHandler.h"
#include "../../Configuration.h"

#include "../RawMessage.h"

class SettingProcess {
   
protected:
    float targetValue = 0;

    unsigned long processStartTime = 0;
    unsigned long processTime = 0;

public:
    virtual void start(const float);
    virtual bool checkFinished();

    void setTargetValue(float targetValue) {this->targetValue = targetValue;}
    float getTargetValue() {return this->targetValue; }
    virtual RawMessage generateMessage();
    
    int getRemainingTime();

    static inline const float upSpeed = 1.0f / UP_TIME;
    static inline const float downSpeed = 1.0f / DOWN_TIME;
};

#endif