#include "dayswitches.h"
#include <QDebug>
#include <QTextEdit>

DaySwitches::DaySwitches(QWidget *parent) : QWidget(parent) {
    horizontalLayout = new QHBoxLayout(this);
    //daySwitch = new DaySwitch*[7];
    dayCheckBoxes = new QCheckBox*[7];

    int width = 0;

    for (int i = 0; i < 7; i++) {
        //daySwitch[i] = new DaySwitch(this);
        //daySwitch[i]->setProperAbbreviation(Timing::getProperAbbreviation(i));
        //daySwitch[i]->setState(true);

        //horizontalLayout->addWidget(daySwitch[i]);
        //horizontalLayout->setAlignment(daySwitch[i], Qt::AlignVCenter);
        //width += daySwitch[i]->width();

        dayCheckBoxes[i] = new QCheckBox(Timing::getProperAbbreviation(i) + ":", this);
        dayCheckBoxes[i]->setCheckState(Qt::Checked);
        horizontalLayout->addWidget(dayCheckBoxes[i]);
        dayCheckBoxes[i]->setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        //horizontalLayout->addWidget(new QTextEdit(this));
    }

    show();

    //setStyleSheet("background-color:magenta;");
    setLayout(horizontalLayout);

    //qInfo()<<"itemHeight2: "<<QString::number(daySwitch[0]->height())<<", 3: "<<QString::number(height());
}

void DaySwitches::updateDaySwitches(bool* daysStates) {
    for (int i = 0; i < 7; i++) {
        if (daysStates[i]) {
            dayCheckBoxes[i]->setCheckState(Qt::Checked);
        }
        else {
            dayCheckBoxes[i]->setCheckState(Qt::Unchecked);
        }
    }
}

std::string DaySwitches::getDaysString() {
    std::string daysString = "";

    for (int i = 0; i < 7; i++) {
        if (dayCheckBoxes[i]->checkState() == Qt::Checked) {
            daysString += "T";
        }
        else {
            daysString += "F";
        }
    }

    return daysString;
}

bool* DaySwitches::getDayStates() {
    for (int i = 0; i < 7; i++) {
        if (dayCheckBoxes[i]->checkState() == Qt::Checked) {
            dayStates[i] = true;
        }
        else {
            dayStates[i] = false;
        }
    }

    return dayStates;
}
