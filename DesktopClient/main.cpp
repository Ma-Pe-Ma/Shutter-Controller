#include "mainwindow/mainwindow.h"

#include <QApplication>
#include <QPushButton>
#include <QTranslator>

#include <timing.h>

int main(int argc, char *argv[]) {
    qRegisterMetaType<std::string>(); 

    QApplication a(argc, argv);

    //Initializing languageFile
    QTranslator translator;

    QString languageFile = "translation_";
    languageFile.append(QLocale().bcp47Name());
    languageFile.append(".qm");

    if (translator.load(languageFile, ":/app/languages")) {
        a.installTranslator(&translator);
    }
    else if (translator.load("translation_en.qm", ":/app/languages")) {
        a.installTranslator(&translator);
    }

    Timing::initializeTimings();

    MainWindow w;

    w.show();
    return a.exec();
}
