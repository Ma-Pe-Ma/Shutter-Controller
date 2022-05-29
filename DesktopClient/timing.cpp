#include "timing.h"
#include <QDebug>

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
    dayAbbreviations[0] = QObject::tr("w0");
    dayAbbreviations[1] = QObject::tr("w1");
    dayAbbreviations[2] = QObject::tr("w2");
    dayAbbreviations[3] = QObject::tr("w3");
    dayAbbreviations[4] = QObject::tr("w4");
    dayAbbreviations[5] = QObject::tr("w5");
    dayAbbreviations[6] = QObject::tr("w6");
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

        int hour = item.value()["H"].get<int>() < 0 ? 0 : item.value()["H"].get<int>();
        int minute = item.value()["M"].get<int>() < 0 ? 0 : item.value()["M"].get<int>();
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
