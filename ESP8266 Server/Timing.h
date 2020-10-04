#ifndef TIMING_H
#define TIMING_H

#include "SettingProcess.h"
#include <ArduinoJson.h>

class Timing : public SettingProcess {

    int hour = -1;
    int minute = -1;
    bool days[7] = {};
    bool queued = false;
    bool active = false;

public:
    static Timing timings[];
    static void CheckTimings(int, int, int);
    static void CreateJsonObject(JsonObject&);

    void setHour(int hour) {this->hour = hour;}
    void setMinute(int minute) {this->minute = minute;}
    void setDays(String daysString) {
        for (int i = 0; i < daysString.length(); i++) {
            if (daysString.charAt(i) == 'T') {
                days[i] = true;
            }
            else {
                days[i] = false;
            }
        }
    }
    void setActive(bool active) {
        this->active = active;
    }

    void generateMessage();
};

#endif