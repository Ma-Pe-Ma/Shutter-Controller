#ifndef PROCESSQUEUE_H
#define PROCESSQUEUE_H

#include <ArduinoQueue.h>

#include "SettingProcess.h"
#include "ZeroProcess.h"

#include "Shutter.pb.h"

class ProcessQueue {
    Shutter_CurrentState currentState;
    SettingProcess currentSettingByClient;
    ZeroProcess zeroProcess;

    SettingProcess* currentProcess = nullptr;

    ArduinoQueue<SettingProcess*> settingQueue = ArduinoQueue<SettingProcess*>(NR_OF_PROCESSES);
    
    std::function<void(Shutter_Event, int)> messageProcessor;
public:
    void processQueue();
    int getQueueCount();
    
    void addSettingToQueue(SettingProcess* newSetting);
    void processZero(Shutter_Zero);

    SettingProcess* getCurrentSettingProcess();
    float getCurrentValue() { return this->currentState.value; }
    void setClientValue(float value);

    void initialize();
    String getLastSetDate() { return String(currentState.day) + "-" + String(currentState.hour) + ":" + String(currentState.minute);}
    
    void saveCurrentStateToFlash();
    void loadCurrentStateFromFlash();

    void setMessageProcessor(std::function<void(Shutter_Event, int)> messageProcessor) {this->messageProcessor = messageProcessor; }

    Shutter_CurrentState& getCurrentState() {return this->currentState; }
};

#endif