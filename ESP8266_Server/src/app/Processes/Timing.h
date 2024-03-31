#ifndef TIMING_H
#define TIMING_H

#include "SettingProcess.h"
#include "../TimeCalibration.h"

class Timing : public SettingProcess {
    bool queued = false;
    int8_t id = -1;

    Shutter_Timing timing = Shutter_Timing_init_default;
public:
    void setID(int8_t id) {this->id = id;}


    void setQueued(bool queued) {this->queued = queued;}
    bool getQueued() { return this->queued; }

    inline std::tuple<Shutter_Event, int> generateMessage() override {
        //TODO:  String(id + 1)
        return {Shutter_Event_timing, (int) (targetValue * 100)};
    }

    Shutter_Timing getTiming() {
        return this->timing;
    }

    void setTiming(Shutter_Timing timing) {
        this->timing = timing;
    }
};

#endif