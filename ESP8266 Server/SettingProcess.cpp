#include "SettingProcess.h"

float SettingProcess::currentValue = 0;
float SettingProcess::upSpeed = UP;
float SettingProcess::downSpeed = DOWN;

SettingProcess SettingProcess::currentSettingByClient;
SettingProcess* SettingProcess::currentProcess = nullptr;

ArduinoQueue<SettingProcess*> SettingProcess::settingQueue = ArduinoQueue<SettingProcess*>(4);

void SettingProcess::start() {
    processStartTime = millis();

    if (value > currentValue) {
        processTime = (value - currentValue) / upSpeed;
        digitalWrite(0, HIGH);

        //Zeroing
        if (value == 1.0f) {
            processTime += 3;
        }
    }
    
    if (value < currentValue) {
        processTime = (currentValue - value) / downSpeed;
 
        //Zeroing
        if (value == 0) {
            processTime += 3;
        }

        digitalWrite(2, HIGH);
    }
}


bool SettingProcess::checkFinished() {
    unsigned long curMillis = millis();

    if (value == currentValue) {
        return true;
    }
    else if (curMillis - processStartTime > processTime) {
        digitalWrite(0, LOW);
        digitalWrite(2, LOW);
        currentValue = value;   
        return true;
    }

    return false;
}

void SettingProcess::processQueue() {
    if (currentProcess == nullptr && !settingQueue.isEmpty()) {
        currentProcess = settingQueue.dequeue();
        currentProcess -> start();
    }
    else if (currentProcess != nullptr) {
        if (currentProcess -> checkFinished()) {
            currentProcess->generateMessage();
            currentProcess = nullptr;
        }
    }
}

void SettingProcess::addSettingToQueue(SettingProcess* newSetting) {
    settingQueue.enqueue(newSetting);
}

SettingProcess* SettingProcess::getCurrentSettingProcess() {
    return currentProcess;
}

float SettingProcess::getCurrentValue() {
    return currentValue;
}

void SettingProcess::setClientValue(float value) {
    currentSettingByClient.setValue(value);
    addSettingToQueue(&currentSettingByClient);
}

void SettingProcess::generateMessage() {
    StaticJsonDocument<2048> doc;

    String message = "";
    serializeJson(doc, message);
    MessageHandler::AddNewMessage(message);
}