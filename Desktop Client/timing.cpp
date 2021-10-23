#include "timing.h"
#include <QDebug>
#include <languages/languages.h>

Timing Timing::timings[NR_OF_TIMINGS];
int Timing::restartTime = 0;

Timing::Timing() {
    time.setHMS(12,0,0);

    for (int i = 0; i < 7; i++) {
        dayStates[i] = false;
    }

    active = rand() % 2;
}

QString Timing::dayAbbreviations[7];

void Timing::initializeTimings() {
    dayAbbreviations[0] = Languages::getFormattedStringByID(Languages::StringResource::w0).c_str();
    dayAbbreviations[1] = Languages::getFormattedStringByID(Languages::StringResource::w1).c_str();
    dayAbbreviations[2] = Languages::getFormattedStringByID(Languages::StringResource::w2).c_str();
    dayAbbreviations[3] = Languages::getFormattedStringByID(Languages::StringResource::w3).c_str();
    dayAbbreviations[4] = Languages::getFormattedStringByID(Languages::StringResource::w4).c_str();
    dayAbbreviations[5] = Languages::getFormattedStringByID(Languages::StringResource::w5).c_str();
    dayAbbreviations[6] = Languages::getFormattedStringByID(Languages::StringResource::w6).c_str();
}

QString Timing::getProperAbbreviation(int dayID) {
    return dayAbbreviations[dayID];
}

Timing* Timing::getTiming(int timingID) {
    return &timings[timingID];
}


void Timing::parseTimings(json timingObject) {
    for (const auto& item : timingObject.items()) {
        int ID = std::stoi(item.key());
        Timing* timing = Timing::getTiming(ID);

        int hour = item.value()["H"].get<int>();
        int minute = item.value()["M"].get<int>();
        bool active = item.value()["A"].get<bool>();
        std::string days = item.value()["D"].get<std::string>();
        int value = item.value()["V"].get<int>();

        bool dayStates[7] = {};

        for (int i = 0; i < 7; i++) {
            if (days[i] == 'T') {
                dayStates[i] = true;
            }
            else {
                dayStates[i] = false;
            }
        }

        timing->setActive(active);
        timing->setTime(QTime(hour, minute));
        timing->setDayStates(dayStates);
        timing->setValue(value);
    }
}

int Timing::currentValue = 0;
int Timing::getCurrentValue() {
    return Timing::currentValue;
}

void Timing::setCurrentValue(int currentValue) {
    Timing::currentValue = currentValue;
}

void Timing::setRestartTime(int restartTime) {
    Timing::restartTime = restartTime;
}

int Timing::getRestartTime() {
    return Timing::restartTime;
}
