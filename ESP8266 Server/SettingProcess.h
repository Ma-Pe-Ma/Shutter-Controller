#ifndef SETTINGPROCESS_H
#define SETTINGPROCESS_H

#define NR_OF_TIMINGS 6
#define UP 0.1f
#define DOWN 0.1f

#include <ArduinoQueue.h>
#include "MessageHandler.h"

class SettingProcess {
    static SettingProcess currentSettingByClient;
    static SettingProcess* currentProcess;

protected:
    static float currentValue;
    static float upSpeed;
    static float downSpeed;
    static ArduinoQueue<SettingProcess*> settingQueue;

    virtual void start();
    virtual bool checkFinished();
    float value = 0;

    unsigned long processStartTime = 0;
    unsigned long processTime = 0;

public:
    static void processQueue();
    static void addSettingToQueue(SettingProcess* newSetting);
    void setValue(float value) {this->value = value;}
    static SettingProcess* getCurrentSettingProcess();
    static float getCurrentValue();
    static void setClientValue(float value);
    virtual void generateMessage();
};

#endif