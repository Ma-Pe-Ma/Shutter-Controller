#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QProgressBar>

#include "requests/dumpgetrequest.h"
#include "requests/timingpostrequest.h"
#include "requests/statusgetrequest.h"
#include "requests/setpostrequest.h"
#include "requests/zeropostrequest.h"

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
    QWidget* central;
    DumpGetRequest* dumpGetRequest = nullptr;
    StatusGetRequest* statusGetRequest = nullptr;
    TimingPostRequest* timingPostRequest = nullptr;
    SetPostRequest* setPostRequest = nullptr;
    ZeroPostRequest* zeroPostRequest = nullptr;
    StatusGetRequest* shortStatusGetRequest = nullptr;

    QTimer* statusTimer = nullptr;

    Ui::MainWindow *ui;
    QPushButton* timingButton = nullptr;
    QProgressBar* progressBar = nullptr;
    QPushButton* upButton = nullptr;
    QPushButton* downButton = nullptr;
    QPushButton* setButton = nullptr;
    QPushButton* zeroButton = nullptr;

    TimingDialog* timingDialog = nullptr;
    SetDialog* setDialog = nullptr;
    ZeroDialog* zeroDialog = nullptr;
    MessageList* messageList = nullptr;
    QLabel* startupDate = nullptr;
    QLabel* currentStateLabel = nullptr;
    QLabel* progressLabel = nullptr;

    void disableGUI();
    void enableGUI();
    void processGenericResponse();

    unsigned int failureCounter = 0;

signals:
    void setUp();
    void setDown();

private slots:
    void dumpGetRequestStart();
    void statusGetRequestStart();
    void statusGetRequestEnd(bool);
    void dumpGetRequestEnd(bool);

    void zeroPostRequestStart(Zero);
    void zeroPostRequestEnd(bool);

    void sendUp();
    void sendDown();

    void setPostRequestStart(int);
    void setPostRequestEnd(bool);

    void shortStatusGetRequestStart();

public slots:
    void timingPostRequestStart(QString);
    void timingPostRequestEnd(bool);
    //void genericResponseReceived(QString);
    void shortStatusGetRequestEnd(bool);
};
#endif // MAINWINDOW_H
