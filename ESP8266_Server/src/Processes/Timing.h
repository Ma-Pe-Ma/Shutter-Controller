#ifndef TIMING_H
#define TIMING_H

#include "SettingProcess.h"
#include "../TimeCalibration.h"
#include <ArduinoJson.h>

class Timing : public SettingProcess {
    int8_t hour = -1;
    int8_t minute = -1;
    bool days[7] = {};
    bool queued = false;
    bool active = false;

public:
    static Timing timings[];
    static Timing timingsBuffer[];
    static void CheckTimings(int, int, int); 
    static void initialize();
    static String readTimingsFromFlash();
    static void loadTimingsFromFlash();
    static void saveTimingsToFlash(String);
    static void CreateJsonObject(JsonObject);
    static void parseTimings(JsonObject&);
    static void disableEarlierSettings();
    

    void setHour(int8_t hour) {this->hour = hour;}
    void setMinute(int8_t minute) {this->minute = minute;}
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

    int8_t getHour() {
        return this->hour;
    }

    int8_t getMinute() {
        return this->minute;
    }

    bool getActivityByDay(int day) {
        return days[day];
    }

    void generateMessage();
};

#endif