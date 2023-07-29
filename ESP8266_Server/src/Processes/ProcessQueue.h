#ifndef PROCESSQUEUE_H
#define PROCESSQUEUE_H

#include <ArduinoQueue.h>

#include "SettingProcess.h"
#include "ZeroProcess.h"

class ProcessQueue {
    SettingProcess currentSettingByClient;
    ZeroProcess zeroProcess;

    SettingProcess* currentProcess = nullptr;

    float currentValue = -1.0f;

    ArduinoQueue<SettingProcess*> settingQueue = ArduinoQueue<SettingProcess*>(NR_OF_PROCESSES);

    int8_t lastSetHour = -1;
    int8_t lastSetMin = -1;
    int8_t lastSetDay = -1;
    
    std::function<void(RawMessage)> messageProcessor;
public:
    void processQueue();
    int getQueueCount();
    
    void addSettingToQueue(SettingProcess* newSetting);
    void processZero(ZeroState);

    SettingProcess* getCurrentSettingProcess();
    float getCurrentValue();
    void setCurrentValue(float);
    void setClientValue(float value);

    void initialize();
    int8_t getLastSetHour() { return lastSetHour;}
    int8_t getLastSetMin() { return lastSetMin;}
    int8_t getLastSetDay() { return lastSetDay;}
    String getLastSetDate() { return String(lastSetDay) + "-" + String(lastSetHour) + ":" + String(lastSetMin);}
    
    void saveCurrentStateToFlash();
    void loadCurrentStateFromFlash();

    void setMessageProcessor(std::function<void(RawMessage)> messageProcessor) {this->messageProcessor = messageProcessor; }
};

#endif