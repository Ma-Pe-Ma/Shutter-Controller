#include "timingitemwidget.h"
#include <QDebug>

TimingItemWidget::TimingItemWidget(int ID, QWidget *parent) : QWidget(parent) {
    this->ID = ID;

    setGeometry(0, 0, 200, 200);

    QHBoxLayout* horizontalLayout = new QHBoxLayout(this);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();

    int itemHeight = height / (NR_OF_TIMINGS + 3) / 2;

    //qInfo()<<"itemHeight: "<<QString::number(itemHeight);

    setFixedSize(width / 3, itemHeight);

    QLabel* qLabel = new QLabel();
    qLabel->setText(QString("%1.").arg(this->ID + 1));
    QFont labelFont = QFont("ARIAL", itemHeight * 0.6);
    qLabel->setFont(labelFont);
    qLabel->setGeometry(0,0, itemHeight,itemHeight);
    qLabel->show();
    horizontalLayout->addWidget(qLabel);
    horizontalLayout->setAlignment(qLabel, Qt::AlignVCenter);

    qTimeEdit = new QTimeEdit(this);
    qTimeEdit->setGeometry(200, 0, 70, 60);
    QFont font("Arial", 20);
    qTimeEdit->setFont(font);
    qTimeEdit->show();
    horizontalLayout->addWidget(qTimeEdit);
    horizontalLayout->setAlignment(qTimeEdit, Qt::AlignVCenter);

    daySwitches = new DaySwitches(this);
    //daySwitches->setGeometry(0,0, daySwitches->width(), itemHeight);
    horizontalLayout->addWidget(daySwitches);
    horizontalLayout->setAlignment(daySwitches, Qt::AlignVCenter);

    valueBox = new QVBoxLayout(this);
    setterValueLabel = new QLabel("?", this);
    valueBox->addWidget(setterValueLabel);
    valueBox->setAlignment(setterValueLabel, Qt::AlignHCenter);

    setter = new QSlider(this);
    setter->setMinimum(0);
    setter->setMaximum(100);
    setter->setOrientation(Qt::Horizontal);
    connect(setter, &QSlider::valueChanged, this, &TimingItemWidget::sliderChanged);
    valueBox->addWidget(setter);

    horizontalLayout->addItem(valueBox);

    toggleSwitch = new ToggleSwitch(this);
    horizontalLayout->addWidget(toggleSwitch);
    horizontalLayout->setAlignment(toggleSwitch, Qt::AlignVCenter);
}

void TimingItemWidget::updateTimingItemWidget() {
    Timing* timing = Timing::getTiming(this->ID);

    qTimeEdit->setTime(timing->getTime());

    if (timing->getActive()) {
        toggleSwitch->setCheckState(Qt::Checked);
    }
    else {
        toggleSwitch->setCheckState(Qt::Unchecked);
    }

    daySwitches->updateDaySwitches(timing->getDayStates());

    setter->setValue(timing->getValue());
    setterValueLabel->setText(QString::number(timing->getValue())+"%");
}

int TimingItemWidget::getHour() {
    return qTimeEdit->time().hour();
}

int TimingItemWidget::getMinute() {
    return qTimeEdit->time().minute();
}

bool TimingItemWidget::getActiveState() {
    if (toggleSwitch->checkState() == Qt::Checked) {
        return true;
    }

    return false;
}

bool* TimingItemWidget::getDayStates() {
    return daySwitches->getDayStates();
}

std::string TimingItemWidget::getDayStateSerialized() {
    return daySwitches->getDaysString();
}

int TimingItemWidget::getValue() {
    return setter->value();
}

void TimingItemWidget::sliderChanged(int value) {
    setterValueLabel->setText(QString::number(value) + "%");
}
