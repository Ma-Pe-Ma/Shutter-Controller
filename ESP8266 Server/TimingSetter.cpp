#include "TimingSetter.h"

TimingSetter TimingSetter::timingSetter;

void TimingSetter::start() {
    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        Timing::timings[i] = buffer[i];
    }
}


bool TimingSetter::checkFinished() {
    queued = false;
    return true;
}

void TimingSetter::processMessage(JsonObject& object) {
    queued = true;

    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        String ID = "" + i;
        JsonObject timingObject = object[ID].as<JsonObject>();

        buffer[i].setValue(timingObject["VALUE"].as<float>());
        buffer[i].setHour(timingObject["HOUR"].as<int>());
        buffer[i].setMinute(timingObject["MIN"].as<int>());
        buffer[i].setDays(timingObject["ACT"].as<String>());
    }

    addSettingToQueue(this);
}

void TimingSetter::generateMessage() {
    StaticJsonDocument<2048> doc;

    String message = "";
    serializeJson(doc, message);
    MessageHandler::AddNewMessage(message);
}
