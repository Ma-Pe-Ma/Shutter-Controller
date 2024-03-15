#ifndef TIMING_H
#define TIMING_H

#include "SettingProcess.h"
#include "../TimeCalibration.h"

#include "../Shutter.pb.h"

class Timing : public SettingProcess {
    bool queued = false;
    int8_t id = -1;

    Shutter_Timing timing = Shutter_Timing_init_default;
public:
    int8_t getID() { return this->id; }
    void setID(int8_t id) {this->id = id;}

    int8_t getHour() { return timing.hour; }
    void setHour(int8_t hour) {timing.hour = hour; }

    int8_t getMinute() { return timing.minute; }
    void setMinute(int8_t minute) {timing.minute = minute; }

    void setActive(bool active) { timing.active = active; }
    bool getActive() { return timing.active; }

    bool getActivityByDay(int day) { return timing.days[day]; }

    void setQueued(bool queued) {this->queued = queued;}
    bool getQueued() { return this->queued; }

    inline std::tuple<Shutter_Event, int> generateMessage() override {
        //TODO:  String(id + 1)
        return {Shutter_Event_timing, (int) (targetValue * 100)};
    }

    Shutter_Timing getTiming() {
        return this->timing;
    }
};

#endif