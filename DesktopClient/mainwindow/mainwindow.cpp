#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QDebug>

#include <QApplication>
#include <QScreen>
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

    setWindowTitle(tr("appName"));
    setWindowIcon(QIcon(":/app/appIcon"));

    central = new QWidget;
    setCentralWidget(central);

    QScreen *screen = QGuiApplication::primaryScreen();
    //QRect screenGeometry = screen->geometry();
    //int height = screenGeometry.height();
    //int width = screenGeometry.width();

    setFixedSize(640,460);
    //setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    //statusBar()->setSizeGripEnabled(false);

    QHBoxLayout* mainBox = new QHBoxLayout;
    QVBoxLayout* progressBox = new QVBoxLayout;

    int buttonWidth = 80;

    //setting up upButton and request
    upButton = new QPushButton(this);
    upButton->setText(tr("up"));
    upButton->setFixedWidth(buttonWidth);
    progressBox->addWidget(upButton);
    connect(upButton, &QPushButton::clicked, this, &MainWindow::setUp);
    connect(this, &MainWindow::setUp, this, &MainWindow::sendUp);

    //barbox
    QHBoxLayout* barBox = new QHBoxLayout(this);

    //setting up progressBar
    progressBar = new QProgressBar(this);
    progressBar->setOrientation(Qt::Vertical);
    progressBar->setFixedSize(20, 220);

    progressBar->setValue(0);
    progressBar->setStyleSheet("QProgressBar::chunk { background-color: #05B8CC; }");
    progressBar->setTextVisible(false);


    barBox->addWidget(progressBar);
    barBox->setAlignment(progressBar, Qt::AlignHCenter);

    progressBox->addLayout(barBox);
    progressBox->setAlignment(barBox, Qt::AlignHCenter);

    //setting up setButton and setDialog
    setDialog = new SetDialog(this);
    setButton = new QPushButton(this);
    setButton->setFixedWidth(buttonWidth / 2);
    std::string setString = "  0%";
    setButton->setText(setString.c_str());
    barBox->addWidget(setButton);
    connect(setButton, &QPushButton::clicked, setDialog, &QDialog::exec);
    connect(setDialog, &SetDialog::setPostRequstStart, this, &MainWindow::setPostRequestStart);

    //setting up downButton and request
    downButton = new QPushButton(this);
    downButton->setText(tr("down"));
    downButton->setFixedWidth(buttonWidth);
    progressBox->addWidget(downButton);
    connect(downButton, &QPushButton::clicked, this, &MainWindow::setDown);
    connect(this, &MainWindow::setDown, this, &MainWindow::sendDown);

    //setting up timingButton and timingDialog
    timingDialog = new TimingDialog(this);

    timingButton = new QPushButton(tr("timings"), this);
    //timingButton->setFixedSize(80,30);
    //timingButton->setAutoFillBackground(true);
    //QPalette pal = timingButton->palette();
    //pal.setColor(QPalette::Button, QColor(Qt::blue));
    //timingButton->setPalette(pal);
    timingButton->update();
    timingButton->setFixedWidth(buttonWidth);
    progressBox->addWidget(timingButton);

    connect(timingButton, &QPushButton::clicked, timingDialog, &QDialog::exec);
    connect(timingDialog, &TimingDialog::timingPostRequestStart, this, &MainWindow::timingPostRequestStart);

    //setting up zeroButton and zeroDialog
    zeroButton = new QPushButton(this);
    zeroButton->setText(tr("nulling"));
    zeroButton->setFixedWidth(buttonWidth);
    progressBox->addWidget(zeroButton);
    zeroDialog = new ZeroDialog(this);
    connect(zeroButton, &QPushButton::clicked, zeroDialog, &QDialog::exec);
    connect(zeroDialog, &ZeroDialog::zeroPostRequestStart, this, &MainWindow::zeroPostRequestStart);

    //setting up MessageBox
    messageList = new MessageList(this);
    startupDate = new QLabel(tr("startTime%1").arg(""), this);

    currentStateLabel = new QLabel(tr("syncing"), this);

    QVBoxLayout* messageBox = new QVBoxLayout;
    messageBox->addWidget(new QLabel(tr("messages")));
    messageBox->addSpacing(0);
    messageBox->addWidget(messageList);

    QHBoxLayout* stateBox = new QHBoxLayout;
    stateBox->addWidget(startupDate);
    stateBox->addWidget(currentStateLabel);
    messageBox->addLayout(stateBox);

    //finalizing main layout
    mainBox->addLayout(progressBox);
    mainBox->addLayout(messageBox);
    central->setLayout(mainBox);

    connect(&Request::requestQueue, &RequestQueue::notifyMessage, this, &MainWindow::notifyMessage);

    //get dump and set up periodic update
    dumpGetRequestStart();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::sendUp() {
    setPostRequestStart(100);
}

void MainWindow::sendDown() {
    setPostRequestStart(0);
}

void MainWindow::enableGUI() {
    central->setEnabled(true);
}

void MainWindow::disableGUI() {
    central->setEnabled(false);
    currentStateLabel->setText(tr("syncing"));
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

    if (empty) {
        Request::requestQueue.enqueueRequest(new StatusGetRequest);
        //failureCounter = 0;
        currentStateLabel->setText(tr("syncing"));
    }
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
    messageList->updateList();
    startupDate->setText(Messages::getStartupDate());
    int restartTime = Timing::getRestartTime();

    int currentValueInt = Timing::getCurrentValue();
    progressBar->setValue(currentValueInt);
    QString progressText = QString::number(currentValueInt) + "%";
    if (progressText.length() < 100) {
        if(progressText.length() > 9) {
            progressText = " " + progressText;
        }
        else {
            progressText = "  " + progressText;
        }
    }
    setButton->setText(progressText);

    if (restartTime > 0) {
        QTimer::singleShot(restartTime * 1000, this, SLOT(statusGetRequestStart()));
        currentStateLabel->setText(tr("syncing"));
        disableGUI();
    }
    else { 
        currentStateLabel->setText(tr("serverAvailable"));
        enableGUI();
    }
}

void MainWindow::notifyMessage(std::string response) {
    if (response == "") {
        currentStateLabel->setText(tr("serverUnavailable"));

        if (failureCounter++ == 5) {
            QCoreApplication::quit();
        }

        if (!initialized) {
            QTimer::singleShot(10000, this, SLOT(dumpGetRequestStart()));
        }
    }
    else {
        failureCounter = 0;

        json responseObject = json::parse(response);
        bool isDump = responseObject.contains("G");

        if (isDump) {
            initialized = true;
            json timingObject = responseObject["T"].get<json>();
            json genericResponse = responseObject["G"].get<json>();

            Timing::parseTimings(timingObject);
            Messages::parseGenericResponse(genericResponse);

            if (statusTimer == nullptr) {
                statusTimer = new QTimer(this);
                connect(statusTimer, &QTimer::timeout, this, &MainWindow::statusGetRequestStart);
                statusTimer->start(30000);
            }
        }
        else {
            Messages::parseGenericResponse(responseObject);
        }

        processGenericResponse();
    }
}