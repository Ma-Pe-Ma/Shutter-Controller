#include "ProcessQueue.h"
#include "../TimeCalibration.h"

#include "../pb_encode.h"
#include "../pb_decode.h"

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
            std::tuple<Shutter_Event, int> processMessage = currentProcess->generateMessage();
            this->messageProcessor(std::get<0>(processMessage), std::get<1>(processMessage));
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
    Shutter_CurrentState currentState = Shutter_CurrentState_init_default;
    currentState.value = currentValue;
    currentState.day = lastSetDay;
    currentState.hour = lastSetHour;
    currentState.minute = lastSetMin;

    uint8_t buffer[20];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, 20);
    
    if (!pb_encode(&stream, Shutter_CurrentState_fields, &currentState)) {
                
    }
    
    esp_yield();
    LittleFSHandler::writeFile("state.txt", buffer, stream.bytes_written);
}

void ProcessQueue::loadCurrentStateFromFlash() {
    String currentStateSerialized;
    LittleFSHandler::readFile("state.txt", currentStateSerialized);

    if (currentStateSerialized != "") {
        Shutter_CurrentState currentState = Shutter_CurrentState_init_default;

        pb_istream_t istream = pb_istream_from_buffer((const unsigned char*) currentStateSerialized.c_str(), currentStateSerialized.length());
        if (!pb_decode(&istream, Shutter_CurrentState_fields, &currentState)) {

        }
 
        currentValue = currentState.value;
        lastSetDay = currentState.day;
        lastSetHour = currentState.hour;
        lastSetMin = currentState.minute;;
    }
    else {
        this->setCurrentValue(0.5f);        
        processZero(Shutter_Zero_current);
    }
}

void ProcessQueue::processZero(Shutter_Zero zeroState) {
    switch (zeroState) {
        case Shutter_Zero_up:
            currentValue = 1.0f;
            TimeCalibration::getCurrentTime(lastSetDay, lastSetHour, lastSetMin);
            saveCurrentStateToFlash();
            break;
        case Shutter_Zero_down:
            currentValue = 0.0f;
            TimeCalibration::getCurrentTime(lastSetDay, lastSetHour, lastSetMin);
            saveCurrentStateToFlash();
            break;
        default:
            settingQueue.enqueue(&zeroProcess);
            break;
    }
}