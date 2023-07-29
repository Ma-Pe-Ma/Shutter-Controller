#include "TimingContainer.h"

void TimingContainer::initialize(ProcessQueue* processQueue) {
    loadTimingsFromFlash();
    this->processQueue = processQueue;
}

void TimingContainer::checkTimings(int day, int hour, int minute) {
    if (hour == 0 && minute == 0) {
        for (int i = 0; i < NR_OF_TIMINGS; i++) {
            timings[i].setQueued(false);
        }
    }

    int currentMinuteTime = 60 * hour + minute;

    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        Timing& checkableTiming = timings[i];

        int timingMinute = 60 * checkableTiming.getHour() + checkableTiming.getMinute();

        if (checkableTiming.getActive() && checkableTiming.getActivityByDay(day) && !checkableTiming.getQueued()) {
            //normal behaviour, spring DST change not implemented
            //only one case, when clock goes from 2 to 3 at the end of march!
            if (timingMinute <= currentMinuteTime) {
                checkableTiming.setQueued(true);
                timingsBuffer[i] = checkableTiming;
                processQueue->addSettingToQueue(&timingsBuffer[i]);
                Serial.println("Timing " + String(i) + " queued!");
            }
        }
    }
}

void TimingContainer::readTimingsFromFlash(String& target) {
    LittleFSHelper::readFile("timings.txt", target);
}

void TimingContainer::saveTimingsToFlash(const String& timingsString) {
    LittleFSHelper::writeFile("timings.txt", timingsString);
}

void TimingContainer::loadTimingsFromFlash() {
    String timingsSerialized;
    LittleFSHelper::readFile("timings.txt", timingsSerialized);

    if (timingsSerialized != "") {
        StaticJsonDocument<1024> doc;

        DeserializationError err = deserializeJson(doc, timingsSerialized);

        if (err == DeserializationError::Ok) {
            JsonObject timingsObject = doc.as<JsonObject>();
            this->parseTimings(timingsObject);
        }
        else {
            Serial.print("Error parsing timing json: ");
            Serial.println(err.c_str());
        }        

        Timing* possibleMissedTiming = nullptr;
        int8_t currentDay = 0;
        int8_t currentHour = 0;
        int8_t currentMinute = 0;

        TimeCalibration::getCurrentTime(currentDay, currentHour, currentMinute);        

        int currentSumMinutes = currentHour * 60 + currentMinute;

        for (int i = 0; i < NR_OF_TIMINGS; i++) {
            Timing& timing = timings[i];

            int timingSumMinutes = timing.getHour() * 60 + timing.getMinute(); 

            if (timing.getActivityByDay(currentDay) && timing.getActive() && timingSumMinutes <= currentSumMinutes) {
                if (possibleMissedTiming == nullptr) {
                    possibleMissedTiming = &timing;
                }
                else {
                    int otherMinutes = possibleMissedTiming->getHour() * 60 + possibleMissedTiming->getMinute();
                    if (timingSumMinutes > otherMinutes) {
                        possibleMissedTiming = &timing;
                    }
                }

                timing.setQueued(true);
            }
        }

        if (possibleMissedTiming != nullptr) {
            int latestSettingMinutes = processQueue->getLastSetHour() * 60 + processQueue->getLastSetMin();
            int possibleMinutes = possibleMissedTiming->getHour() * 60 + possibleMissedTiming->getMinute();

            if (processQueue->getLastSetDay() != currentDay || possibleMinutes > latestSettingMinutes) {
                for (int i = 0; i < NR_OF_TIMINGS; i++) {
                    if (possibleMissedTiming == &timings[i]) {
                        timingsBuffer[i] = timings[i];
                        processQueue->addSettingToQueue(&timingsBuffer[i]);
                    }
                }
            }
        }
    }
    else {
        StaticJsonDocument<1024> doc;
        JsonObject object = doc.to<JsonObject>();
        createJsonObject(object);

        String serialized;
        serializeJson(doc, serialized);
        //Serial.println("Saving: " + serialized);
        saveTimingsToFlash(serialized);
        doc.clear();
        //Serial.println("Timings saved to flash: "+serialized);
    }
}

void TimingContainer::parseTimings(JsonObject& timingsObject) {
    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        Timing& modifiableTiming = timings[i];

        String ID = String(i);
        JsonObject timingObject = timingsObject[ID].as<JsonObject>();

        int intValue = timingObject["V"].as<int>();
        modifiableTiming.setTargetValue(1.0f * intValue / 100);
        modifiableTiming.setHour(timingObject["H"].as<int>());
        modifiableTiming.setMinute(timingObject["M"].as<int>());
        modifiableTiming.parseDayStates(timingObject["D"].as<String>());
        modifiableTiming.setActive(timingObject["A"].as<bool>());
        modifiableTiming.setID(i);
    }
}

void TimingContainer::createJsonObject(JsonObject& object) {
    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        String ID = String(i);
        Timing& timing = timings[i];
        timing.setID(i);
        JsonObject timingObject = object.createNestedObject(ID);
        timingObject["V"] = int (timing.getTargetValue() * 100);
        timingObject["H"] = timing.getHour();
        timingObject["M"] = timing.getMinute();
        timingObject["A"] = timing.getActive();
        timingObject["D"] = timing.serializeDayStates();
    }
}

void TimingContainer::disableEarlierSettings() {
    int8_t currentDay = 0;
    int8_t currentHour = 0;
    int8_t currentMinute = 0;

    TimeCalibration::getCurrentTime(currentDay, currentHour, currentMinute);

    int currentSumMinutes = currentHour * 60 + currentMinute;

    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        Timing& timing = timings[i];
        
        if (timing.getActivityByDay(currentDay)) {
            int timingMinute = timing.getHour() * 60 + timing.getMinute();

            if (timingMinute <= currentSumMinutes) {
                timing.setQueued(true);
            }
            else {
                timing.setQueued(false);
            }
        }
    }
}