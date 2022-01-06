#include "ZeroProcess.h"
#include "../TimeCalibration.h"

ZeroProcess ZeroProcess::zeroProcess;

void ZeroProcess::processNull(ZeroState zeroState) {
    zeroProcess.zeroState = zeroState;

    switch (zeroState) {
        case up:
            currentValue = 1.0f;
            TimeCalibration::GetCurrentTime(lastSetDay, lastSetHour, lastSetMin);
            saveCurrentStateToFlash();
            break;
        case down:
            currentValue = 0.0f;
            TimeCalibration::GetCurrentTime(lastSetDay, lastSetHour, lastSetMin);
            saveCurrentStateToFlash();
            break;
        default:
            settingQueue.enqueue(&zeroProcess);
            break;
    }    
}

void ZeroProcess::start() {
    processStartTime = millis();
    digitalWrite(UP_PIN, DEACTIVATE_PIN);
    digitalWrite(DOWN_PIN, DEACTIVATE_PIN);

    zeroFound = false;
    targetValue = currentValue;

    if (currentValue < 0.5f) {
        processTime = 1.0f / downSpeed;
        digitalWrite(DOWN_PIN, ACTIVATE_PIN);
    }
    else {
        processTime = 1.0f / upSpeed;
        digitalWrite(UP_PIN, ACTIVATE_PIN);
    }

    processTime += 3.0f;

    Serial.println("Zero process started, process time: " + String(processTime));
}
    
bool ZeroProcess::checkFinished() {
    if (!zeroFound) {
        if (millis() - processStartTime > processTime * 1000) {   
            if (targetValue < 0.5f) {
                digitalWrite(DOWN_PIN, DEACTIVATE_PIN);
                digitalWrite(UP_PIN, ACTIVATE_PIN);
                processTime = targetValue / upSpeed;
            }
            else {
                digitalWrite(UP_PIN, DEACTIVATE_PIN);
                digitalWrite(DOWN_PIN, ACTIVATE_PIN);                
                processTime = (1.0f - targetValue) / downSpeed;
            }

            zeroFound = true;
            processStartTime = millis(); 

            Serial.println("Zeroing half, new processs time: " + String(processTime));
        }
    }
    else {
        if (millis() - processStartTime > processTime * 1000) {
            digitalWrite(UP_PIN, DEACTIVATE_PIN);
            digitalWrite(DOWN_PIN, DEACTIVATE_PIN);

            TimeCalibration::GetCurrentTime(lastSetDay, lastSetHour, lastSetMin);

            currentValue = targetValue;

            Serial.println("Zeroing finished: " + String(currentValue));
            return true;
        }
    }

    return false;
}

void ZeroProcess::generateMessage() {
    int intCurrent = (int) (currentValue * 100);
    MessageHandler::AddNewMessage("S", "Z", String(intCurrent));
}