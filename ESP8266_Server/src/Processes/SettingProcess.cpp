#include "SettingProcess.h"
#include "../TimeCalibration.h"
#include "../LittleFSHelper.h"
#include "ZeroProcess.h"

void SettingProcess::start(const float currentValue) {
    processStartTime = millis();
    processTime = 0;

    Serial.println("Target value: " + String(targetValue) + ", current: " + String(currentValue));

    digitalWrite(UP_PIN, DEACTIVATE_PIN);
    digitalWrite(DOWN_PIN, DEACTIVATE_PIN);

    if (targetValue > currentValue) {
        processTime = (targetValue - currentValue) / upSpeed;

        Serial.println("Up - difference: " + String(targetValue - currentValue) + ", time: " + String(processTime));

        //Zeroing
        if (targetValue == 1.0f) {
            processTime += 3;
        }

        digitalWrite(UP_PIN, ACTIVATE_PIN);
    }
    else if (targetValue < currentValue) {
        processTime = (currentValue - targetValue) / downSpeed;
 
        Serial.println("Down - difference: " + String(currentValue - targetValue) + ", time: " + String(processTime));

        //Zeroing
        if (targetValue == 0) {
            processTime += 3;
        }

        digitalWrite(DOWN_PIN, ACTIVATE_PIN);
    }
    else {
        processTime = 0;
    }

    Serial.println("Generic Process started: " + String(processTime));
}

bool SettingProcess::checkFinished() {
    unsigned long curMillis = millis();

    if (processTime == 0) {        
        Serial.println("Setting finished, unchanged: " + String(targetValue));

        digitalWrite(UP_PIN, DEACTIVATE_PIN);
        digitalWrite(DOWN_PIN, DEACTIVATE_PIN);
        return true;
    }
    else if (curMillis - processStartTime > processTime * 1000) {
        Serial.println("Setting finished: " + String(targetValue));

        digitalWrite(UP_PIN, DEACTIVATE_PIN);
        digitalWrite(DOWN_PIN, DEACTIVATE_PIN);
        return true;
    }

    return false;
}

RawMessage SettingProcess::generateMessage() {
    int intCurrent = (int) (targetValue * 100);

    return {"S", "M", String(intCurrent)};
}

int SettingProcess::getRemainingTime() { 
    return int(-(millis() - processStartTime)) / 1000 + processTime + 1;
}