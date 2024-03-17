#include "ProcessQueue.h"
#include "../TimeCalibration.h"

#include "../pb_encode.h"
#include "../pb_decode.h"

void ProcessQueue::processQueue() { 
    if (currentProcess == nullptr && !settingQueue.isEmpty()) {
        currentProcess = settingQueue.dequeue();
        currentProcess->start(currentState.value);
        Serial.println("New process dequeud!");      
    }
    else if (currentProcess != nullptr) {
        //Serial.println("In progress....");
    
        if (currentProcess->checkFinished()) {
            currentState.value = currentProcess->getTargetValue();
            TimeCalibration::getCurrentTime(currentState.day, currentState.hour, currentState.minute);

            this->saveCurrentStateToFlash();
            esp_yield();
            std::tuple<Shutter_Event, int> processMessage = currentProcess->generateMessage();
            this->messageProcessor(std::get<0>(processMessage), std::get<1>(processMessage));
            currentProcess = nullptr;
            Serial.println("Process finished: " + String(currentState.value));
        }
    }
}

void ProcessQueue::initialize() {
    ProcessQueue::settingQueue = ArduinoQueue<SettingProcess*>(10);

    //Load state from before blackout, else zero to middle!
    loadCurrentStateFromFlash();

    Serial.println("Queue size: " + String(settingQueue.itemSize()) + ", count: " + String(settingQueue.itemCount()));
}

void ProcessQueue::addSettingToQueue(SettingProcess* newSetting) {
    settingQueue.enqueue(newSetting);
}

SettingProcess* ProcessQueue::getCurrentSettingProcess() {
    return currentProcess;
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
    uint8_t buffer[Shutter_CurrentState_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, Shutter_CurrentState_size);
    
    if (pb_encode(&stream, Shutter_CurrentState_fields, &currentState)) {
        LittleFSHandler::writeFile("state.txt", buffer, stream.bytes_written);
    }
}

void ProcessQueue::loadCurrentStateFromFlash() {
    String currentStateSerialized;
    LittleFSHandler::readFile("state.txt", currentStateSerialized);

    if (currentStateSerialized != "") {
        currentState = Shutter_CurrentState_init_default;

        pb_istream_t istream = pb_istream_from_buffer((const unsigned char*) currentStateSerialized.c_str(), currentStateSerialized.length());
        if (!pb_decode(&istream, Shutter_CurrentState_fields, &currentState)) {
            LittleFSHandler::deleteFile("state.txt");
        }
    }
    else {
        currentState.value = 0.5f;
        processZero(Shutter_Zero_current);
    }
}

void ProcessQueue::processZero(Shutter_Zero zeroState) {
    switch (zeroState) {
        case Shutter_Zero_up:
            currentState.value = 1.0f;
            TimeCalibration::getCurrentTime(currentState.day, currentState.hour, currentState.minute);
            saveCurrentStateToFlash();
            break;
        case Shutter_Zero_down:
            currentState.value = 0.0f;
            TimeCalibration::getCurrentTime(currentState.day, currentState.hour, currentState.minute);
            saveCurrentStateToFlash();
            break;
        default:
            settingQueue.enqueue(&zeroProcess);
            break;
    }
}