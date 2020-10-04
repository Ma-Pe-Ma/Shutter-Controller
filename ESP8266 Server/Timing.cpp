#include "Timing.h"
#include "TimingSetter.h"

Timing Timing::timings[NR_OF_TIMINGS];

void Timing::CheckTimings(int day, int hour, int minute) {
    int modDay = day - 1;
    if (modDay == -1) {
        modDay = 6;
    }

    if (hour == 0 && minute == 0) {
        for (int i = 0; i < NR_OF_TIMINGS; i++) {
            timings[i].queued = false;
        }    
    }

    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        Timing* currentTiming = &timings[i];

        if (currentTiming->days[modDay] == true && currentTiming->active
            && currentTiming->queued == false && TimingSetter::timingSetter.queued == false) {
            //normal behaviour : first two conditions
            if ((currentTiming->hour == hour && currentTiming->minute == minute)) {
                currentTiming->queued = true;
                addSettingToQueue(currentTiming);
            }
            //DST change behaviour, only on case per year
            else if (hour > currentTiming->hour) {
                currentTiming->queued = true;
                addSettingToQueue(currentTiming);
            }
        }
    }
}

void Timing::CreateJsonObject(JsonObject& object) {
    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        String ID = "" + i;
        Timing* timing = &Timing::timings[i];
        JsonObject timingObject = object.createNestedObject(ID);
        timingObject["VALUE"] = timing->value;
        timingObject["HOUR"] = timing->hour;
        timingObject["MIN"] = timing->minute;
        timingObject["ACT"] = timing->active;
    }
}

void Timing::generateMessage() {
    StaticJsonDocument<2048> doc;

    String message = "";
    serializeJson(doc, message);
    MessageHandler::AddNewMessage(message);
}