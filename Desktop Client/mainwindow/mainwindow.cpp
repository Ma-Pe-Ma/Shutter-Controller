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

#include <languages/languages.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    std::string appNameString = Languages::getFormattedStringByID(Languages::StringResource::appName);

    setWindowTitle(appNameString.c_str());
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

    //setting up upButton and request
    upButton = new QPushButton(this);
    std::string upString = Languages::getFormattedStringByID(Languages::StringResource::up);
    upButton->setText(upString.c_str());
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

    progressLabel = new QLabel(this);
    barBox->addWidget(progressLabel);

    progressBox->addLayout(barBox);
    progressBox->setAlignment(barBox, Qt::AlignHCenter);

    //setting up downButton and request
    downButton = new QPushButton(this);
    std::string downString = Languages::getFormattedStringByID(Languages::StringResource::down);
    downButton->setText(downString.c_str());
    progressBox->addWidget(downButton);
    connect(downButton, &QPushButton::clicked, this, &MainWindow::setDown);
    connect(this, &MainWindow::setDown, this, &MainWindow::sendDown);

    //setting up setButton and setDialogpro
    setDialog = new SetDialog(this);
    setButton = new QPushButton(this);
    std::string setString = Languages::getFormattedStringByID(Languages::StringResource::setPos);
    setButton->setText(setString.c_str());
    progressBox->addWidget(setButton);
    connect(setButton, &QPushButton::clicked, setDialog, &QDialog::exec);
    connect(setDialog, &SetDialog::setPostRequstStart, this, &MainWindow::setPostRequestStart);

    //setting up timingButton and timingDialog
    timingDialog = new TimingDialog(this);

    std::string timingsString = Languages::getFormattedStringByID(Languages::StringResource::timings);
    timingButton = new QPushButton(timingsString.c_str(), this);
    timingButton->setFixedSize(80,30);
    timingButton->setAutoFillBackground(true);
    //QPalette pal = timingButton->palette();
    //pal.setColor(QPalette::Button, QColor(Qt::blue));
    //timingButton->setPalette(pal);
    timingButton->update();
    progressBox->addWidget(timingButton);

    connect(timingButton, &QPushButton::clicked, timingDialog, &QDialog::exec);
    connect(timingDialog, &TimingDialog::timingPostRequestStart, this, &MainWindow::timingPostRequestStart);

    //setting up zeroButton and zeroDialog
    zeroButton = new QPushButton(this);
    std::string zeroString = Languages::getFormattedStringByID(Languages::StringResource::nulling);
    zeroButton->setText(zeroString.c_str());
    progressBox->addWidget(zeroButton);
    zeroDialog = new ZeroDialog(this);
    connect(zeroButton, &QPushButton::clicked, zeroDialog, &QDialog::exec);
    connect(zeroDialog, &ZeroDialog::zeroPostRequestStart, this, &MainWindow::zeroPostRequestStart);

    //setting up MessageBox
    messageList = new MessageList(this);
    std::string startupString = Languages::getFormattedStringByID(Languages::StringResource::startTime, "-");
    startupDate = new QLabel(startupString.c_str(), this);

    std::string syncString = Languages::getFormattedStringByID(Languages::StringResource::syncing);
    currentStateLabel = new QLabel(syncString.c_str(), this);

    QVBoxLayout* messageBox = new QVBoxLayout;
    std::string messagesString = Languages::getFormattedStringByID(Languages::StringResource::messages);
    messageBox->addWidget(new QLabel(messagesString.c_str()));
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
    std::string sync = Languages::getFormattedStringByID(Languages::StringResource::syncing);
    qInfo()<<"sync: "<<sync.length();
    currentStateLabel->setText(sync.c_str());
}

//------DUMP GET------

void MainWindow::dumpGetRequestStart() {
    disableGUI();

    if (dumpGetRequest != nullptr) {
        return;
    }

    dumpGetRequest = new DumpGetRequest;
    connect(dumpGetRequest, &DumpGetRequest::dumpGetEnd, this, &MainWindow::dumpGetRequestEnd);
    dumpGetRequest->start();
}

void MainWindow::dumpGetRequestEnd(bool success) {
    if (dumpGetRequest != nullptr) {
        while(dumpGetRequest->isRunning());
        disconnect(dumpGetRequest, &DumpGetRequest::dumpGetEnd, this, &MainWindow::dumpGetRequestEnd);
        delete dumpGetRequest;
        dumpGetRequest = nullptr;
    }

    if (success) {
        processGenericResponse();

        if (statusTimer == nullptr) {
            statusTimer = new QTimer(this);
            connect(statusTimer, &QTimer::timeout, this, &MainWindow::statusGetRequestStart);
            statusTimer->start(30000);
        }

        failureCounter = 0;
        std::string serverAvailableString = Languages::getFormattedStringByID(Languages::StringResource::serverAvailable);
        currentStateLabel->setText(serverAvailableString.c_str());
    }
    else {
        if (failureCounter++ == 3) {
            QCoreApplication::quit();
        }

        std::string serverUnavailableString = Languages::getFormattedStringByID(Languages::StringResource::serverUnavailable);
        currentStateLabel->setText(serverUnavailableString.c_str());

        QTimer::singleShot(10000, this, SLOT(dumpGetRequestStart()));
        return;
    }
}

//------STATUS GET------

void MainWindow::statusGetRequestStart() {
    if (statusGetRequest == nullptr && dumpGetRequest == nullptr && setPostRequest == nullptr &&
            timingPostRequest == nullptr && shortStatusGetRequest == nullptr && zeroPostRequest == nullptr) {
        statusGetRequest = new StatusGetRequest;
        connect(statusGetRequest, &StatusGetRequest::statusGetEnd, this, &MainWindow::statusGetRequestEnd);
        statusGetRequest->start();
        failureCounter = 0;
        std::string syncString = Languages::getFormattedStringByID(Languages::StringResource::syncing);
        currentStateLabel->setText(syncString.c_str());
    }
    else {

    }
}

void MainWindow::statusGetRequestEnd(bool success) {
    if (statusGetRequest != nullptr) {
        disconnect(statusGetRequest, &StatusGetRequest::statusGetEnd, this, &MainWindow::statusGetRequestEnd);
        delete statusGetRequest;
        statusGetRequest = nullptr;
    }

    if (success) {
        processGenericResponse();
    }
    else {
        std::string serverUnavailableString = Languages::getFormattedStringByID(Languages::StringResource::serverUnavailable);
        currentStateLabel->setText(serverUnavailableString.c_str());
    }
}

//------SET POST------

void MainWindow::setPostRequestStart(int value) { 
    if (setPostRequest == nullptr) {
        setPostRequest = new SetPostRequest(value);
        connect(setPostRequest, &SetPostRequest::setPostEnd, this, &MainWindow::setPostRequestEnd);
        setPostRequest->start();
        disableGUI();
    }
    else {
        return;
    }
}

void MainWindow::setPostRequestEnd(bool success) {
    if (setPostRequest != nullptr) {
        disconnect(setPostRequest, &SetPostRequest::setPostEnd, this, &MainWindow::setPostRequestEnd);
        delete setPostRequest;
        setPostRequest = nullptr;
    }

    if (success) {
        processGenericResponse();
    }
    else {

    }
}

//------TIMING POST------

void MainWindow::timingPostRequestStart(QString timingString) {
    json timingObject = json::parse(timingString.toUtf8().constData());

    if (timingPostRequest == nullptr) {
        timingPostRequest = new TimingPostRequest(timingObject);
        connect(timingPostRequest, &TimingPostRequest::timingPostEnd, this, &MainWindow::timingPostRequestEnd);
        timingPostRequest->start();
        disableGUI();
    }
    else {
        return;
    }
}

void MainWindow::timingPostRequestEnd(bool success) {
    if (timingPostRequest != nullptr) {
        disconnect(timingPostRequest, &TimingPostRequest::timingPostEnd, this, &MainWindow::timingPostRequestEnd);
        delete timingPostRequest;
        timingPostRequest = nullptr;
    }

    if (success) {
        timingDialog->updateTimingsFromGui();
        processGenericResponse();
    }
    else {
        std::string serverUnavailableString = Languages::getFormattedStringByID(Languages::StringResource::serverUnavailable);
        currentStateLabel->setText(serverUnavailableString.c_str());
    }
}

//------ZERO POST------

void MainWindow::zeroPostRequestStart(Zero zero) {
    if (zeroPostRequest == nullptr) {
        zeroPostRequest = new ZeroPostRequest(zero);
        connect(zeroPostRequest, &ZeroPostRequest::zeroPostEnd, this, &MainWindow::zeroPostRequestEnd);
        zeroPostRequest->start();
        disableGUI();
    }
    else {
        return;
    }
}

void MainWindow::zeroPostRequestEnd(bool success) {
    if (zeroPostRequest != nullptr) {
        disconnect(zeroPostRequest, &ZeroPostRequest::zeroPostEnd, this, &MainWindow::zeroPostRequestEnd);
        delete zeroPostRequest;
        zeroPostRequest = nullptr;
    }

    if (success) {
        processGenericResponse();
    }
    else {
        std::string serverUnavailableString = Languages::getFormattedStringByID(Languages::StringResource::serverUnavailable);
        currentStateLabel->setText(serverUnavailableString.c_str());
    }
}

//------SHORT STATUS POST------

void MainWindow::shortStatusGetRequestStart() {
    if (shortStatusGetRequest == nullptr) {
        shortStatusGetRequest = new StatusGetRequest();
        connect(shortStatusGetRequest, &StatusGetRequest::statusGetEnd, this, &MainWindow::shortStatusGetRequestEnd);
        shortStatusGetRequest->start();
        disableGUI();
    }
    else {
        return;
    }
}

void MainWindow::shortStatusGetRequestEnd(bool success) {
    if (shortStatusGetRequest != nullptr) {
        disconnect(shortStatusGetRequest, &StatusGetRequest::statusGetEnd, this, &MainWindow::shortStatusGetRequestEnd);
        delete shortStatusGetRequest;
        shortStatusGetRequest = nullptr;
    }

    if (success) {
        processGenericResponse();
    }
    else {
        std::string serverUnavailableString = Languages::getFormattedStringByID(Languages::StringResource::serverUnavailable);
        currentStateLabel->setText(serverUnavailableString.c_str());
    }
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
    progressLabel->setText(progressText);

    if (restartTime > 0) {
        QTimer::singleShot(restartTime * 1000, this, SLOT(shortStatusGetRequestStart()));
    }
    else {
        std::string serverAvailableString = Languages::getFormattedStringByID(Languages::StringResource::serverAvailable);
        currentStateLabel->setText(serverAvailableString.c_str());
        enableGUI();
    }
}
