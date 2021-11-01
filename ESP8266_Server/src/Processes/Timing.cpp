#include "Timing.h"
#include "../LittleFSHelper.h"
#include "../TimeCalibration.h"

Timing Timing::timings[NR_OF_TIMINGS];
Timing Timing::timingsBuffer[NR_OF_TIMINGS];

void Timing::CheckTimings(int day, int hour, int minute) {
    if (hour == 0 && minute == 0) {
        for (int i = 0; i < NR_OF_TIMINGS; i++) {
            timings[i].queued = false;
        }
    }

    int currentMinuteTime = 60 * hour + minute;

    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        Timing* checkableTiming = &timings[i];

        int timingMinute = 60 * checkableTiming->hour + checkableTiming->minute;

        if (checkableTiming->active && checkableTiming->days[day] && !checkableTiming->queued) {
            
            Serial.println("timingminute: " +String(timingMinute) + ", cur: " + String(currentMinuteTime));

            //normal behaviour, spring DST change not implemented
            //only one case, when clock goes from 2 to 3 at the end of march!
            if (timingMinute <= currentMinuteTime) {
                checkableTiming->queued = true;

                timingsBuffer[i] = *checkableTiming;
                addSettingToQueue(&timingsBuffer[i]);
                Serial.println("Timing " +String(i) +" queued!");
            }
        }
    }
}

void Timing::initialize() {
    loadTimingsFromFlash();
}

String Timing::readTimingsFromFlash() {
    return LittleFSHelper::readFile("timings.txt");
}

void Timing::saveTimingsToFlash(String timingsString) {
    LittleFSHelper::writeFile("timings.txt", timingsString);
}

void Timing::loadTimingsFromFlash() {
    String timingsSerialized = LittleFSHelper::readFile("timings.txt");

    Serial.println("Loading timings: " + timingsSerialized);

    if (timingsSerialized != "") {
        StaticJsonDocument<1024> doc;

        deserializeJson(doc, timingsSerialized);
        JsonObject timingsObject = doc.to<JsonObject>();
        Timing::parseTimings(timingsObject);

        Timing* possibleMissedTiming = nullptr;
        int8_t currentDay = 0;
        int8_t currentHour = 0;
        int8_t currentMinute = 0;

        TimeCalibration::GetCurrentTime(currentDay, currentHour, currentMinute);        

        int currentSumMinutes = currentHour * 60 + currentMinute;

        for (int i = 0; i < NR_OF_TIMINGS; i++) {
            Timing* timing = &timings[i];

            int timingSumMinutes = timing->hour * 60 + timing->minute; 

            if (timing->days[currentDay] && timing->active && timingSumMinutes <= currentSumMinutes) {
                if (possibleMissedTiming == nullptr) {
                    possibleMissedTiming = timing;
                }
                else {
                    int otherMinutes = possibleMissedTiming->hour * 60 + possibleMissedTiming->minute;
                    if (timingSumMinutes > otherMinutes) {
                        possibleMissedTiming = timing;
                    }
                }
            }
        }

        if (possibleMissedTiming != nullptr) {
            int latestSettingMinutes = SettingProcess::getLastSetHour() * 60 + SettingProcess::getLastSetMin();
            int possibleMinutes = possibleMissedTiming->hour * 60 + possibleMissedTiming->minute;

            if (SettingProcess::getLastSetDay() != currentDay || possibleMinutes > latestSettingMinutes) {
                for (int i = 0; i < NR_OF_TIMINGS; i++) {
                    if (possibleMissedTiming == &timings[i]) {
                        timingsBuffer[i] = timings[i];
                        addSettingToQueue(&timingsBuffer[i]);
                    }
                }
            }
        }
    }
    else {
        StaticJsonDocument<1024> doc;
        JsonObject object = doc.to<JsonObject>();
        CreateJsonObject(object);

        String serialized;
        serializeJson(doc, serialized);
        //Serial.println("Saving: " + serialized);
        saveTimingsToFlash(serialized);
        doc.clear();
        Serial.println("Timings saved to flash: "+serialized);
    }
}

void Timing::CreateJsonObject(JsonObject object) {
    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        String ID = String(i);
        Timing* timing = &Timing::timings[i];
        JsonObject timingObject = object.createNestedObject(ID);
        timingObject["V"] = int (timing->targetValue * 100);
        timingObject["H"] = timing->hour;
        timingObject["M"] = timing->minute;
        timingObject["A"] = timing->active;

        String daysString = "";

        for (int i = 0; i < 7; i++) {
            if (timing->days[i]) {
                daysString += 'T';
            }
            else {
                daysString += 'F';
            }
        }

        timingObject["D"] = daysString;
    }
}

void Timing::parseTimings(JsonObject& timingsObject) {
    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        Timing* modifiableTiming = &Timing::timings[i];

        String ID = String(i);
        JsonObject timingObject = timingsObject[ID].as<JsonObject>();

        int intValue = timingObject["V"].as<int>();
        modifiableTiming->setTargetValue(1.0f * intValue / 100);
        modifiableTiming->setHour(timingObject["H"].as<int>());
        modifiableTiming->setMinute(timingObject["M"].as<int>());
        modifiableTiming->setDays(timingObject["D"].as<String>());
        modifiableTiming->setActive(timingObject["A"].as<bool>());
    }
}

void Timing::disableEarlierSettings() {
    int8_t currentDay = 0;
    int8_t currentHour = 0;
    int8_t currentMinute = 0;

    TimeCalibration::GetCurrentTime(currentDay, currentHour, currentMinute);

    int currentSumMinutes = currentHour * 60 + currentMinute;

    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        Timing* timing = &timings[i];
        
        if (timing->days[currentDay]) {
            int timingMinute = timing->hour * 60 + timing->minute;

            if (timingMinute <= currentSumMinutes) {
                timing->queued = true;
            }
            else {
                timing->queued = false;
            }
        }
    }
}

void Timing::generateMessage() {
    int ID = 0;
    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        if (this == &timingsBuffer[i]) {
            ID = i + 1;
            break;
        }
    }

    int intCurrent = (int) (currentValue * 100);
    MessageHandler::AddNewMessage("T", String(ID), String(intCurrent));
}