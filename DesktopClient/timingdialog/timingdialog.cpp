#include <QDebug>

#include "timingdialog.h"
#include "mainwindow/mainwindow.h"

TimingDialog::TimingDialog(QWidget* parent) : QDialog(parent) {
    //QScreen *screen = QGuiApplication::primaryScreen();
    //QRect  screenGeometry = screen->geometry();
    //int height = screenGeometry.height();
    //int width = screenGeometry.width();

    int dividerWidth = 0;
    //int listElementHeight = height / (NR_OF_TIMINGS + 3) / 2;
    int listElementHeight = 60;
    int dialogHeight = listElementHeight * (NR_OF_TIMINGS + 1) + 10;
    //int dialogWidth = width / 3;
    int dialogWidth = 640;

    verticalLayout = new QVBoxLayout(this);

    setWindowTitle(tr("timings"));
    setFixedSize(dialogWidth + 20, dialogHeight);

    qListWidget = new QListWidget(this);
    qListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    qListWidget->setStyleSheet(QString("QListWidget::item { border-bottom: %1px solid black; }").arg(dividerWidth));
    qListWidget->setStyleSheet(QString("QListView::item { height: %1px; }").arg(listElementHeight));
    qListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    qListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    timingItemWidgets = new TimingItemWidget*[NR_OF_TIMINGS];
    qListWidgetItems = new QListWidgetItem*[NR_OF_TIMINGS];

    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        timingItemWidgets[i] = new TimingItemWidget(i, this);
        qListWidgetItems[i] = new QListWidgetItem();

        timingItemWidgets[i]->setFixedSize(dialogWidth, listElementHeight);
        qListWidgetItems[i]->setSizeHint(QSize(dialogWidth, listElementHeight));

        qListWidget->addItem(qListWidgetItems[i]);
        qListWidget->setItemWidget(qListWidgetItems[i], timingItemWidgets[i]);
    }

    int listHeight = qListWidget->sizeHintForRow(0) * qListWidget->count() + 2 * qListWidget->frameWidth();
    qListWidget->setFixedSize(dialogWidth+20, listHeight +20);


    qListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    qListWidget->setFocusPolicy(Qt::NoFocus);
    verticalLayout->addWidget(qListWidget);

    okButton = new QPushButton(this);
    okButton->setText(tr("setButton"));
    //okButton->setFixedWidth(20);
    //okButton->resize(okButton->sizeHint().width(), okButton->sizeHint().height());
    //okButton->adjustSize();
    //okButton->setFixedSize(okButton->minimumSize());
    verticalLayout->addWidget(okButton);
    verticalLayout->setAlignment(okButton, Qt::AlignRight);

    setLayout(verticalLayout);

    connect(okButton, &QPushButton::clicked, this, &TimingDialog::timingComfirmed);
}

void TimingDialog::showEvent(QShowEvent* event) {
    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        timingItemWidgets[i]->updateTimingItemWidget();
    }

    QWidget::showEvent(event);
}

void TimingDialog::timingComfirmed() {
    json timingObject = createTimingObject();
    emit timingPostRequestStart(QString::fromUtf8(timingObject.dump().c_str()));
    done(0);
}

json TimingDialog::createTimingObject() {
    json timingsObject;

    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        json timingObject;
        TimingItemWidget* timing = timingItemWidgets[i];

        timingObject["H"] = timing->getHour();
        timingObject["M"] = timing->getMinute();
        timingObject["A"] = timing->getActiveState();
        timingObject["D"] = timing->getDayStateSerialized();
        timingObject["V"] = timing->getValue();

        timingsObject[std::to_string(i)] = timingObject;
    }

    return timingsObject;
}

void TimingDialog::updateTimingsFromGui() {
    for (int i = 0; i < NR_OF_TIMINGS; i++) {
        TimingItemWidget* timingWidget = timingItemWidgets[i];
        Timing* timing = Timing::getTiming(i);

        QTime time = QTime(timingWidget->getHour(), timingWidget->getMinute());

        timing->setTime(time);
        timing->setActive(timingWidget->getActiveState());
        timing->setDayStates(timingWidget->getDayStates());
        timing->setValue(timingWidget->getValue());
    }
}
