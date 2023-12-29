#ifndef TIMING_H
#define TIMING_H

#include "SettingProcess.h"
#include "../TimeCalibration.h"
#include <ArduinoJson.h>

class Timing : public SettingProcess {
    int8_t hour = 12;
    int8_t minute = 0;
    bool days[7] = {};
    bool queued = false;
    bool active = false;

    int8_t id = -1;

public:
    int8_t getID() { return this->id; }
    void setID(int8_t id) {this->id = id;}

    int8_t getHour() { return this->hour; }
    void setHour(int8_t hour) {this->hour = hour; }

    int8_t getMinute() { return this->minute; }
    void setMinute(int8_t minute) {this->minute = minute; }

    void parseDayStates(String daysString) {
        int length = daysString.length() > 7 ? 7 : daysString.length();

        for (int i = 0; i < length; i++) {
            days[i] = daysString.charAt(i) == 'T' ? true : false;
        }
    }

    String serializeDayStates() {
        String daysString = "";

        for (int i = 0; i < 7; i++) {
            daysString += days[i] ? 'T' : 'F';
        }

        return daysString;
    }

    void setActive(bool active) { this->active = active; }
    bool getActive() { return this->active; }

    bool getActivityByDay(int day) { return days[day]; }   

    void setQueued(bool queued) {this->queued = queued;}
    bool getQueued() { return this->queued; }

    inline RawMessage generateMessage() override {
        int intCurrent = (int) (targetValue * 100);
        return {"T", String(id + 1), String(intCurrent)};
    }
};

#endif