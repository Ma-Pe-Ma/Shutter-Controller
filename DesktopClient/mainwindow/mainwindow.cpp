#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QDebug>

#include <QApplication>
#include <QDialog>
#include <QListWidget>
#include <QTextEdit>
#include <QListWidgetItem>

#include <QTimer>

#include "requests/request.h"
#include "requests/requestqueue.h"

#include "requests/dumpgetrequest.h"
#include "requests/timingpostrequest.h"
#include "requests/statusgetrequest.h"
#include "requests/setpostrequest.h"
#include "requests/zeropostrequest.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setFixedSize(640,460);
    statusBar()->setSizeGripEnabled(false);

    ui->progressBox->setAlignment(ui->barBox, Qt::AlignHCenter);
    ui->setButton->setFixedWidth(40);

    ui->startupLabel->setText(tr("startTime%1").arg(""));
    ui->stateLabel->setText(tr("syncing"));

    zeroDialog = new ZeroDialog(this);
    setDialog = new SetDialog(this);
    timingDialog = new TimingDialog(this);

    connect(setDialog, &SetDialog::setPostRequstStart, this, &MainWindow::setPostRequestStart);
    connect(timingDialog, &TimingDialog::timingPostRequestStart, this, &MainWindow::timingPostRequestStart);
    connect(zeroDialog, &ZeroDialog::zeroPostRequestStart, this, &MainWindow::zeroPostRequestStart);
    connect(&Request::requestQueue, &RequestQueue::notifyMessage, this, &MainWindow::notifyMessage);

    //get dump and set up periodic update
    dumpGetRequestStart();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::enableGUI() {
    this->centralWidget()->setEnabled(true);
}

void MainWindow::disableGUI() {
    this->centralWidget()->setEnabled(false);
    ui->stateLabel->setText(tr("syncing"));
}

//------DUMP GET------

void MainWindow::dumpGetRequestStart() {
    disableGUI();
    Request::requestQueue.enqueueRequest(new DumpGetRequest);
}

//------STATUS GET------

void MainWindow::statusGetRequestStart() {
    bool empty = true;
    Request::requestQueue.isEmpty(empty);

    if (empty && !longSyncInProgress) {
        Request::requestQueue.enqueueRequest(new StatusGetRequest);
        //failureCounter = 0;
        ui->stateLabel->setText(tr("syncing"));
    }
}

void MainWindow::statusGetRequestRetry() {
    Request::requestQueue.enqueueRequest(new StatusGetRequest);
    ui->stateLabel->setText(tr("syncing"));
}

//------SET POST------

void MainWindow::setPostRequestStart(int value) { 
    Request::requestQueue.enqueueRequest(new SetPostRequest(value));
    disableGUI();
}

//------TIMING POST------

void MainWindow::timingPostRequestStart(QString timingString) {
    json timingObject = json::parse(timingString.toUtf8().constData());
    TimingPostRequest* tpr = new TimingPostRequest(timingObject);
    connect(tpr, &Request::customSignal, this, &MainWindow::updateTimings);
    Request::requestQueue.enqueueRequest(tpr);

    disableGUI();
}

void MainWindow::updateTimings() {
    timingDialog->updateTimingsFromGui();
}

//------ZERO POST------

void MainWindow::zeroPostRequestStart(Zero zero) {
    Request::requestQueue.enqueueRequest(new ZeroPostRequest(zero));
    disableGUI();
}

//------GENERCIC RESPONSE------

void MainWindow::processGenericResponse() {
    ui->messageList->updateList();
    ui->startupLabel->setText(Messages::getStartupDate());
    int restartTime = Timing::getRestartTime();

    int currentValueInt = Timing::getCurrentValue();
    ui->progressBar->setValue(currentValueInt);
    QString progressText = QString::number(currentValueInt) + "%";
    if (progressText.length() < 100) {
        if(progressText.length() > 9) {
            progressText = " " + progressText;
        }
        else {
            progressText = "  " + progressText;
        }
    }
    ui->setButton->setText(progressText);

    if (restartTime > 0) {
        QTimer::singleShot(restartTime * 1000, this, SLOT(statusGetRequestRetry()));
        ui->stateLabel->setText(tr("syncing"));
        longSyncInProgress = true;
        disableGUI();
    }
    else {
        longSyncInProgress = false;
        ui->stateLabel->setText(tr("serverAvailable"));
        enableGUI();
    }
}

void MainWindow::notifyMessage(std::string response) {
    if (response == "") {
        ui->stateLabel->setText(tr("serverUnavailable"));

        if (failureCounter++ == 5) {
            QCoreApplication::quit();
        }

        if (!initialized) {
            QTimer::singleShot(10000, this, SLOT(dumpGetRequestStart()));
        }
        else {
            if (longSyncInProgress) {
                longSyncInProgress = false;
                statusGetRequestStart();
            }
        }
    }
    else {
        failureCounter = 0;

        qInfo()<<"response: "<<response.c_str();

        try {

        json responseObject = json::parse(response);
        bool isDump = responseObject.contains("T");

        if (isDump) {
            initialized = true;
            json timingObject = responseObject["T"].get<json>();
            json genericResponse = responseObject["G"].get<json>();

            if (statusTimer == nullptr) {
                statusTimer = new QTimer(this);
                connect(statusTimer, &QTimer::timeout, this, &MainWindow::statusGetRequestStart);
                statusTimer->start(30000);
            }

            Timing::parseTimings(timingObject);
            Messages::parseGenericResponse(genericResponse);
        }
        else {
            json genericResponse = responseObject["G"].get<json>();
            Messages::parseGenericResponse(genericResponse);
        }

        processGenericResponse();

        }
        catch(nlohmann::json::exception) {
            ui->stateLabel->setText(tr("json error"));

            if (longSyncInProgress) {
                longSyncInProgress = false;
            }

            statusGetRequestStart();
        }
    }
}

void MainWindow::on_upButton_clicked()
{
    setPostRequestStart(100);
}


void MainWindow::on_downButton_clicked()
{
    setPostRequestStart(0);
}


void MainWindow::on_timingsButton_clicked()
{
    timingDialog->exec();
}


void MainWindow::on_setButton_clicked()
{
    setDialog->exec();
}


void MainWindow::on_nullButton_clicked()
{
    zeroDialog->exec();
}
