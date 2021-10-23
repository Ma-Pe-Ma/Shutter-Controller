#include "SettingProcess.h"
#include "../TimeCalibration.h"
#include "../LittleFSHelper.h"
#include "ZeroProcess.h"

float SettingProcess::currentValue = -1.0f;
const float SettingProcess::upSpeed = UP_SPEED;
const float SettingProcess::downSpeed = DOWN_SPEED;

int8_t SettingProcess::lastSetHour = -1;
int8_t SettingProcess::lastSetMin = -1;
int8_t SettingProcess::lastSetDay = -1;

SettingProcess SettingProcess::currentSettingByClient;
SettingProcess* SettingProcess::currentProcess = nullptr;

ArduinoQueue<SettingProcess*> SettingProcess::settingQueue = ArduinoQueue<SettingProcess*>(4);

void SettingProcess::start() {
    processStartTime = millis();
    processTime = 0;

    Serial.println("Target value: " + String(targetValue) + ", current: " + String(currentValue));

    if (targetValue > currentValue) {
        processTime = (targetValue - currentValue) / upSpeed;

        Serial.println("ProcessTime: "+String(processTime));

        //Zeroing
        if (targetValue == 1.0f) {
            processTime += 3;
        }

        digitalWrite(UP_PIN, HIGH);
    }
    
    if (targetValue < currentValue) {
        processTime = (currentValue - targetValue) / downSpeed;
 
        //Zeroing
        if (targetValue == 0) {
            processTime += 3;
        }

        digitalWrite(DOWN_PIN, HIGH);
    }

    Serial.println("Generic Process started: "+String(processTime));
}

bool SettingProcess::checkFinished() {
    unsigned long curMillis = millis();

    if (targetValue == currentValue) {
        TimeCalibration::GetCurrentTime(lastSetDay, lastSetHour, lastSetMin);
        Serial.println("Setting finished, unchanged: " + String(currentValue));
        return true;
    }
    else if (curMillis - processStartTime > processTime * 1000) {
        digitalWrite(UP_PIN, LOW);
        digitalWrite(DOWN_PIN, LOW);
        currentValue = targetValue;

        TimeCalibration::GetCurrentTime(lastSetDay, lastSetHour, lastSetMin);

        Serial.println("Setting finished: " + String(currentValue));

        return true;
    }

    return false;
}

void SettingProcess::initialize() {
    SettingProcess::settingQueue = ArduinoQueue<SettingProcess*>(10);

    //Load state from before blackout, else zero to middle!
    loadCurrentStateFromFlash();

    Serial.println("Queue size: "+String(SettingProcess::settingQueue.itemSize())+", count: "+String(SettingProcess::settingQueue.itemCount()));
}

int8_t SettingProcess::getLastSetHour() {
    return lastSetHour;
}

int8_t SettingProcess::getLastSetMin() {
    return lastSetMin;
}

int8_t SettingProcess::getLastSetDay() {
    return lastSetDay;
}

void SettingProcess::processQueue() { 
    if (currentProcess == nullptr && !settingQueue.isEmpty()) {
        currentProcess = settingQueue.dequeue();
        currentProcess -> start();
        Serial.println("New process dequeud!");      
    }
    else if (currentProcess != nullptr) {
        //Serial.println("In progress....");
    
        if (currentProcess -> checkFinished()) {
            currentProcess->saveCurrentStateToFlash();
            currentProcess->generateMessage();
            currentProcess = nullptr;
            Serial.println("Process finished!");
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

void SettingProcess::setCurrentValue(float currentValue) {
    SettingProcess::currentValue = currentValue;
}

void SettingProcess::setClientValue(float targetValue) {
    currentSettingByClient.setTargetValue(targetValue);
    addSettingToQueue(&currentSettingByClient);
}

int SettingProcess::getQueueCount() {
    if (currentProcess != nullptr) {
        return 1;
    }

    return settingQueue.itemCount();
}

void SettingProcess::generateMessage() {
    int intCurrent = (int) (currentValue * 100);
    MessageHandler::AddNewMessage("S", "M", String(intCurrent));
}

void SettingProcess::saveCurrentStateToFlash() {
    StaticJsonDocument<2048> doc;
    doc["V"] = currentValue;
    doc["D"] = lastSetDay;
    doc["H"] = lastSetHour;
    doc["M"] = lastSetMin;

    String out = "";
    serializeJson(doc, out);
    LittleFSHelper::writeFile("state.txt", out);
}

void SettingProcess::loadCurrentStateFromFlash() {
    String currentState = LittleFSHelper::readFile("state.txt");

    if (currentState != "") {
        StaticJsonDocument<2048> doc;
        deserializeJson(doc, currentState);
 
        currentValue =  doc["V"].as<float>();
        lastSetDay = doc["D"].as<int8_t>();
        lastSetHour = doc["H"].as<int8_t>();
        lastSetMin = doc["M"].as<int8_t>();
    }
    else {
        SettingProcess::setCurrentValue(0.5f);
        ZeroProcess::processNull(ZeroState::find);
    }
}