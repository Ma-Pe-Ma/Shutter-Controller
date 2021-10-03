#include "NullProcess.h"

NullProcess NullProcess::nullProcess;

void NullProcess::processNull(NullState nullState) {
    switch (nullState) {
        case up:
            currentValue = 1.0f;
            break;
        
        case down:
            currentValue = 0.0f;
            break;

        case find:
            settingQueue.enqueue(&nullProcess);
            break;
        default:
            break;
    }
}

void NullProcess::start() {
    processStartTime = millis();
    zeroFound = false;

    if (currentValue < 0.5f) {
        digitalWrite(2, HIGH);
        processTime = 1.0f / downSpeed;
    }
    else {
        digitalWrite(0, HIGH);
        processTime = 1.0f / upSpeed;
    }
}
    
bool NullProcess::checkFinished() {

    if (!zeroFound) {

        if (millis() - processStartTime > processTime) {
            
            if (currentValue < 0.5f) {
                digitalWrite(2, LOW);
                digitalWrite(0, HIGH);
                processTime = value / upSpeed;
            }
            else {
                digitalWrite(0, LOW);
                digitalWrite(2, HIGH);
                processTime = (1.0f - value) / downSpeed;
            }

            processStartTime = millis();
            zeroFound = true;
        }
    }
    else {
         if (millis() - processStartTime > processTime) {
            
            digitalWrite(0, LOW);
            digitalWrite(2, LOW);

            currentValue = value;
            return true;
         }
    }

    return false;
}

void NullProcess::generateMessage() {
    StaticJsonDocument<2048> doc;

    String message = "";
    serializeJson(doc, message);
    MessageHandler::AddNewMessage(message);
}