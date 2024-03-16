#include "TimingContainer.h"

#include "../pb_encode.h"
#include "../pb_decode.h"

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
        Shutter_Timing shutter_timing = checkableTiming.getTiming();

        int timingMinute = 60 * shutter_timing.hour + shutter_timing.hour;

        if (shutter_timing.active && shutter_timing.days[day] && !checkableTiming.getQueued()) {
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

void TimingContainer::loadTimingsFromFlash() {
    String timingsSerialized;
    LittleFSHandler::readFile("timings.txt", timingsSerialized);

    if (timingsSerialized != "") {
        Shutter_Request timingContainer = Shutter_Request_init_default;

        pb_istream_t istream = pb_istream_from_buffer((const unsigned char*) timingsSerialized.c_str(), timingsSerialized.length());
        if(!pb_decode(&istream, Shutter_Request_fields, &timingContainer)) {

        }
        
        this->parseTimings(timingContainer);

        Timing* possibleMissedTiming = nullptr;
        int currentDay = 0;
        int currentHour = 0;
        int currentMinute = 0;

        TimeCalibration::getCurrentTime(currentDay, currentHour, currentMinute);        

        int currentSumMinutes = currentHour * 60 + currentMinute;

        for (int i = 0; i < NR_OF_TIMINGS; i++) {
            Timing& timing = timings[i];
            Shutter_Timing shutter_timing = timing.getTiming();

            int timingSumMinutes = shutter_timing.hour * 60 + shutter_timing.minute;

            if (shutter_timing.days[currentDay] && shutter_timing.active && timingSumMinutes <= currentSumMinutes) {
                if (possibleMissedTiming == nullptr) {
                    possibleMissedTiming = &timing;
                }
                else {
                    Shutter_Timing possible_shutter_timing = possibleMissedTiming->getTiming();
                    int otherMinutes = possible_shutter_timing.hour * 60 + possible_shutter_timing.minute;
                    if (timingSumMinutes > otherMinutes) {
                        possibleMissedTiming = &timing;
                    }
                }

                timing.setQueued(true);
            }
        }

        if (possibleMissedTiming != nullptr) {
            Shutter_CurrentState& currentState = processQueue->getCurrentState();
            int latestSettingMinutes = currentState.hour * 60 + currentState.minute;
            Shutter_Timing possible_shutter_timing = possibleMissedTiming->getTiming();
            int possibleMinutes = possible_shutter_timing.hour * 60 + possible_shutter_timing.minute;

            if (currentState.day != currentDay || possibleMinutes > latestSettingMinutes) {
                for (int i = 0; i < NR_OF_TIMINGS; i++) {
                    if (possibleMissedTiming == &timings[i]) {
                        timingsBuffer[i] = timings[i];
                        processQueue->addSettingToQueue(&timingsBuffer[i]);
                    }
                }
            }
        }
    }
}

void TimingContainer::parseTimings(Shutter_Request& timingContainer) {
    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        Timing& modifiableTiming = timings[i];
        Shutter_Timing& timingObject = timingContainer.timing[i];

        timingObject.value = timingObject.value > 100 ? 100 : (timingObject.value < 0 ? 0 : timingObject.value);
        timingObject.value -= timingObject.value % 5 - (timingObject.value % 5 < 3 ? 0 : 5);

        modifiableTiming.setTargetValue(1.0f * timingObject.value / 100);
        modifiableTiming.setTiming(timingObject);
        modifiableTiming.setID(i);
    }
}

void TimingContainer::disableEarlierSettings() {
    int currentDay = 0;
    int currentHour = 0;
    int currentMinute = 0;

    TimeCalibration::getCurrentTime(currentDay, currentHour, currentMinute);

    int currentSumMinutes = currentHour * 60 + currentMinute;

    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        Timing& timing = timings[i];
        Shutter_Timing shutter_timing = timing.getTiming();
        
        if (shutter_timing.days[currentDay]) {
            int timingMinute = shutter_timing.hour * 60 + shutter_timing.minute;

            if (timingMinute <= currentSumMinutes) {
                timing.setQueued(true);
            }
            else {
                timing.setQueued(false);
            }
        }
    }
}