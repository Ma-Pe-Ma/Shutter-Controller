#include "ZeroProcess.h"

ZeroProcess ZeroProcess::zeroProcess;

void ZeroProcess::processNull(ZeroState zeroState) {
    zeroProcess.zeroState = zeroState;

    switch (zeroState) {
        case up:
            currentValue = 1.0f;
            break;
        case down:
            currentValue = 0.0f;
            break;
        default:
            settingQueue.enqueue(&zeroProcess);
            break;
    }    
}

void ZeroProcess::start() {
    processStartTime = millis();
    digitalWrite(UP_PIN, LOW);
    digitalWrite(DOWN_PIN, LOW);

    zeroFound = false;
    targetValue = currentValue;

    if (currentValue < 0.5f) {
        processTime = 1.0f / downSpeed;
        digitalWrite(DOWN_PIN, HIGH);   
    }
    else {
        processTime = 1.0f / upSpeed;
        digitalWrite(UP_PIN, HIGH);
    }

    processTime += 3.0f;

    Serial.println("Zero process started, process time: " + String(processTime));
}
    
bool ZeroProcess::checkFinished() {
    if (!zeroFound) {
        if (millis() - processStartTime > processTime * 1000) {   
            if (targetValue < 0.5f) {
                digitalWrite(DOWN_PIN, LOW);
                digitalWrite(UP_PIN, HIGH);
                processTime = targetValue / upSpeed;
            }
            else {
                digitalWrite(DOWN_PIN, HIGH);
                digitalWrite(UP_PIN, LOW);
                processTime = (1.0f - targetValue) / downSpeed;
            }

            zeroFound = true;
            processStartTime = millis(); 

            Serial.println("Zeroing half, new processs time: " + String(processTime));
        }
    }
    else {
         if (millis() - processStartTime > processTime * 1000) {
            digitalWrite(UP_PIN, LOW);
            digitalWrite(DOWN_PIN, LOW);

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