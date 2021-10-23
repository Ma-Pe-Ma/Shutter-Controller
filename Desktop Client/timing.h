#ifndef TIMING_H
#define TIMING_H

#define NR_OF_TIMINGS 6
#include <QString>
#include <QTime>
#include "json.hpp"

using json = nlohmann::json;

class Timing {
public:
    Timing();

    static void initializeTimings();
    static QString getProperAbbreviation(int);

    void setActive(bool active) {this->active = active;}
    void setTime(QTime time) {this->time = time;}
    //void setDaysMap(std::map<int, bool> daysMap) {this->daysMap = daysMap;}
    void setDayStates(bool dayStates[7]) {
        //this->dayStates = dayStates;
        memcpy(this->dayStates, dayStates, 7);
    }
    void setValue(int value) {this->value = value;}

    bool getActive() {return this->active;}
    QTime getTime() {return this->time;}

    bool* getDayStates() {return dayStates;}
    int getValue() {return value;}

    static Timing* getTiming(int);
    static void parseTimings(json timings);
    static json serializeTimings();

    static int getCurrentValue();
    static void setCurrentValue(int);

    static int getRestartTime();
    static void setRestartTime(int);

private:
    static Timing timings[NR_OF_TIMINGS];
    static QString dayAbbreviations[7];
    static int restartTime;

    QTime time;
    bool active = false;
    //std::map<int, bool> daysMap;
    bool dayStates[7] = {};
    int value = 0;

    static int currentValue;
};

#endif // TIMING_H
