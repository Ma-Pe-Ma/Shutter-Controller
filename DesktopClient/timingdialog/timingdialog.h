#ifndef TIMINGDIALOG_H
#define TIMINGDIALOG_H

#include <QDialog>
#include <QGuiApplication>
#include <QScreen>
#include <QPushButton>
#include <QListWidget>
#include "timingitemwidget.h"
#include "timing.h"

class MainWindow;

class TimingDialog : public QDialog {
    Q_OBJECT

private:
    QPushButton* qPushButton = nullptr;

    TimingItemWidget** timingItemWidgets = nullptr;
    QListWidgetItem** qListWidgetItems = nullptr;
    QListWidget* qListWidget = nullptr;
    QPushButton* okButton = nullptr;
    QVBoxLayout* verticalLayout = nullptr;

public:
    TimingDialog(QWidget* parent);
    json createTimingObject();
    void updateTimingsFromGui();

signals:
    void timingPostRequestStart(QString);

public slots:
    void timingComfirmed();

protected:
    void showEvent(QShowEvent* event);
};

#endif // TIMINGDIALOG_H
