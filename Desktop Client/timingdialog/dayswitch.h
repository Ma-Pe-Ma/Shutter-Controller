#ifndef DAYSWITCH_H
#define DAYSWITCH_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>

class DaySwitch : public QWidget {
    Q_OBJECT
public:
    explicit DaySwitch(QWidget *parent = nullptr);
    void setProperAbbreviation(QString abbr);
    void setState(bool);

private:
    QLabel* dayLabel = nullptr;
    QCheckBox* dayCheck = nullptr;


signals:

};

#endif // DAYSWITCH_H
