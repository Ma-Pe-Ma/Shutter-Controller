#include "dayswitch.h"
#include <QDebug>


DaySwitch::DaySwitch(QWidget *parent) : QWidget(parent) {
    QVBoxLayout* verticalLayout = new QVBoxLayout();

    int spacing = 5;

    dayLabel = new QLabel(this);
    dayCheck = new QCheckBox(this);

    verticalLayout->addWidget(dayLabel);
    verticalLayout->addWidget(dayCheck);

    verticalLayout->setSpacing(spacing);
    verticalLayout->setContentsMargins(0,0,0,0);
    dayLabel->setMargin(0);
    dayLabel->setContentsMargins(0,0,0,0);
    dayLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    dayCheck->setContentsMargins(0,0,0,0);
    verticalLayout->setAlignment(dayCheck, Qt::AlignHCenter | Qt::AlignBottom);

    setLayout(verticalLayout);
    verticalLayout->setAlignment(Qt::AlignCenter);
    //setStyleSheet("background-color:yellow;");

    QFontMetrics metrics(dayLabel->font());
    setFixedHeight(dayCheck->height() / 3 + metrics.height() + spacing);
}

void DaySwitch::setProperAbbreviation(QString abbr) {
    dayLabel->setText(abbr);
    setFixedWidth(dayLabel->fontMetrics().horizontalAdvance(dayLabel->text()) > dayCheck->width() / 4 ? dayLabel->fontMetrics().horizontalAdvance(dayLabel->text()) : dayCheck->width() / 4);
}
