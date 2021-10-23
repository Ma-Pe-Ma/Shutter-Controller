#ifndef DAYSWITCHES_H
#define DAYSWITCHES_H

#include <QWidget>
#include <QHBoxLayout>
#include "dayswitch.h"
#include "timing.h"
#include <map>

class DaySwitches : public QWidget {
    Q_OBJECT

private:
    QHBoxLayout* horizontalLayout = nullptr;
    //DaySwitch** daySwitch = nullptr;
    QCheckBox** dayCheckBoxes = nullptr;
    bool dayStates[7];

public:
    explicit DaySwitches(QWidget *parent = nullptr);
    void updateDaySwitches(bool* daysStates);
    std::string getDaysString();
    bool* getDayStates();

signals:

};

#endif // DAYSWITCHES_H
