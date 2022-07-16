#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QProgressBar>

#include "messagelist.h"

#include "timingdialog/timingdialog.h"
#include "setdialog/setdialog.h"
#include "zerodialog/zerodialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool initialized = false;
    bool longSyncInProgress = false;

    QTimer* statusTimer = nullptr;

    Ui::MainWindow *ui;

    TimingDialog* timingDialog = nullptr;
    SetDialog* setDialog = nullptr;
    ZeroDialog* zeroDialog = nullptr;

    void disableGUI();
    void enableGUI();
    void processGenericResponse();

    unsigned int failureCounter = 0;

private slots:
    void dumpGetRequestStart();
    void statusGetRequestStart();
    void statusGetRequestRetry();
    void zeroPostRequestStart(Zero);
    void setPostRequestStart(int);
    void timingPostRequestStart(QString);

    void notifyMessage(std::string);
    void updateTimings();

    void on_upButton_clicked();
    void on_downButton_clicked();

    void on_timingsButton_clicked();

    void on_setButton_clicked();

    void on_nullButton_clicked();

public slots:

};
#endif // MAINWINDOW_H
