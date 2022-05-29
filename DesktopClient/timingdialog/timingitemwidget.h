#ifndef TIMINGITEMWIDGET_H
#define TIMINGITEMWIDGET_H

#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <timing.h>
#include <QTimeEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QSlider>

#include "toggleswitch.h"
#include "dayswitches.h"

class TimingItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit TimingItemWidget(int ID, QWidget *parent = nullptr);
    void updateTimingItemWidget();

    int getHour();
    int getMinute();
    bool getActiveState();
    std::string getDayStateSerialized();
    bool* getDayStates();
    int getValue();

private:
    int ID = -1;
    ToggleSwitch* toggleSwitch = nullptr;
    QTimeEdit* qTimeEdit = nullptr;
    DaySwitches* daySwitches = nullptr;

    QVBoxLayout* valueBox = nullptr;
    QLabel* setterValueLabel = nullptr;
    QSlider* setter = nullptr;

signals:

private slots:
    void sliderChanged(int);

};

#endif // TIMINGITEMWIDGET_H
