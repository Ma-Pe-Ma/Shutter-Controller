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
    QWidget* central;
    bool initialized = false;

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
    void zeroPostRequestStart(Zero);
    void setPostRequestStart(int);
    void timingPostRequestStart(QString);

    void notifyMessage(std::string);
    void updateTimings();

    void sendUp();
    void sendDown();
public slots:

};
#endif // MAINWINDOW_H
