#ifndef SETTINGPROCESS_H
#define SETTINGPROCESS_H

#include <ArduinoQueue.h>
#include "../MessageHandler.h"
#include "../../Configuration.h"

class SettingProcess {
    static SettingProcess currentSettingByClient;
    static SettingProcess* currentProcess;

protected:
    static float currentValue;
    const static float upSpeed;
    const static float downSpeed;
    static ArduinoQueue<SettingProcess*> settingQueue;

    virtual void start();
    virtual bool checkFinished();
    float targetValue = 0;

    unsigned long processStartTime = 0;
    unsigned long processTime = 0;

    static int8_t lastSetHour;
    static int8_t lastSetMin;
    static int8_t lastSetDay;

public:
    static int getQueueCount();
    void setTargetValue(float targetValue) {this->targetValue = targetValue;}
    virtual void generateMessage();

    static void processQueue();
    static void addSettingToQueue(SettingProcess* newSetting);
    static SettingProcess* getCurrentSettingProcess();
    static float getCurrentValue();
    static void setCurrentValue(float);
    static void setClientValue(float value);

    static void initialize();
    static int8_t getLastSetHour();
    static int8_t getLastSetMin();
    static int8_t getLastSetDay();
    static void saveCurrentStateToFlash();
    static void loadCurrentStateFromFlash();
};

#endif