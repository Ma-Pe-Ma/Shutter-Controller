#include "mainwindow/mainwindow.h"

#include <QApplication>
#include <QPushButton>
#include <QtDebug>

#include <timing.h>
#include <languages/languages.h>

int main(int argc, char *argv[]) {
    qRegisterMetaType<std::string>();

    QApplication a(argc, argv);

    Languages::initialize();
    Timing::initializeTimings();


    MainWindow w;

    w.show();
    return a.exec();
}
