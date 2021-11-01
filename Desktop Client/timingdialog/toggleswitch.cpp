#include "toggleswitch.h"
#include <QApplication>
#include <QScreen>

ToggleSwitch::ToggleSwitch(QWidget* parent) : QCheckBox(parent) {

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();

    int buttonHeight = 50;

    int listElementHeight = height / (NR_OF_TIMINGS + 3) / 2;

    setGeometry(0,0,listElementHeight,listElementHeight);
    setStyleSheet(QString(
                        //"QCheckBox"
                        //"{"
                        //"    background-color: white;"
                        //"               color: black;"
                        //"}"
                        "QCheckBox::indicator"
                        "{"
                        "width: %1px;"
                        "height: %1px;"
                        //"border: 3px solid #5A5A5A;"
                        //"background: cyan;"
                        "}"
                        "QCheckBox::indicator:checked {"
                        "image: url(:/app/switchOn);"
                        "}"
                        "QCheckBox::indicator:unchecked {"
                        "image: url(:/app/switchOff);"
                        "}"
                        ).arg(listElementHeight-6));
    update();
    show();
}
