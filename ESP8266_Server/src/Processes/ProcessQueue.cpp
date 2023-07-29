#include "ProcessQueue.h"
#include "../TimeCalibration.h"

void ProcessQueue::processQueue() { 
    if (currentProcess == nullptr && !settingQueue.isEmpty()) {
        currentProcess = settingQueue.dequeue();
        currentProcess->start(currentValue);
        Serial.println("New process dequeud!");      
    }
    else if (currentProcess != nullptr) {
        //Serial.println("In progress....");
    
        if (currentProcess->checkFinished()) {
            this->currentValue = currentProcess->getTargetValue();
            TimeCalibration::getCurrentTime(lastSetDay, lastSetHour, lastSetMin);

            this->saveCurrentStateToFlash();
            esp_yield();
            this->messageProcessor(currentProcess->generateMessage());
            currentProcess = nullptr;
            Serial.println("Process finished!");
        }
    }
}

void ProcessQueue::initialize() {
    ProcessQueue::settingQueue = ArduinoQueue<SettingProcess*>(10);

    //Load state from before blackout, else zero to middle!
    loadCurrentStateFromFlash();

    Serial.println("Queue size: "+String(settingQueue.itemSize())+", count: "+String(settingQueue.itemCount()));
}

void ProcessQueue::addSettingToQueue(SettingProcess* newSetting) {
    settingQueue.enqueue(newSetting);
}

SettingProcess* ProcessQueue::getCurrentSettingProcess() {
    return currentProcess;
}

float ProcessQueue::getCurrentValue() {
    return currentValue;
}

void ProcessQueue::setCurrentValue(float currentValue) {
    this->currentValue = currentValue;
}

void ProcessQueue::setClientValue(float targetValue) {
    currentSettingByClient.setTargetValue(targetValue);
    addSettingToQueue(&currentSettingByClient);
}

int ProcessQueue::getQueueCount() {
    if (currentProcess != nullptr) {
        return 1;
    }

    return settingQueue.itemCount();
}

void ProcessQueue::saveCurrentStateToFlash() {
    StaticJsonDocument<256> doc;
    doc["V"] = currentValue;
    doc["D"] = lastSetDay;
    doc["H"] = lastSetHour;
    doc["M"] = lastSetMin;

    String out = "";
    serializeJson(doc, out);
    esp_yield();
    LittleFSHelper::writeFile("state.txt", out);
}

void ProcessQueue::loadCurrentStateFromFlash() {
    String currentState;
    LittleFSHelper::readFile("state.txt", currentState);

    if (currentState != "") {
        StaticJsonDocument<256> doc;
        deserializeJson(doc, currentState);
 
        currentValue =  doc["V"].as<float>();
        lastSetDay = doc["D"].as<int8_t>();
        lastSetHour = doc["H"].as<int8_t>();
        lastSetMin = doc["M"].as<int8_t>();
    }
    else {
        this->setCurrentValue(0.5f);        
        processZero(ZeroState::find);
    }
}

void ProcessQueue::processZero(ZeroState zeroState) {
    switch (zeroState) {
        case ZeroState::up:
            currentValue = 1.0f;
            TimeCalibration::getCurrentTime(lastSetDay, lastSetHour, lastSetMin);
            saveCurrentStateToFlash();
            break;
        case ZeroState::down:
            currentValue = 0.0f;
            TimeCalibration::getCurrentTime(lastSetDay, lastSetHour, lastSetMin);
            saveCurrentStateToFlash();
            break;
        default:
            settingQueue.enqueue(&zeroProcess);
            break;
    }    
}