QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    mainwindow/messagelist.cpp \
    mainwindow/messagewidget.cpp \
    messages.cpp \
    requests/dumpgetrequest.cpp \
    requests/request.cpp \
    requests/requestqueue.cpp \
    requests/setpostrequest.cpp \
    requests/statusgetrequest.cpp \
    requests/timingpostrequest.cpp \
    requests/zeropostrequest.cpp \
    setdialog/setdialog.cpp \
    timingdialog/dayswitch.cpp \
    timingdialog/dayswitches.cpp \
    main.cpp \
    mainwindow/mainwindow.cpp \
    timing.cpp \
    timingdialog/timingdialog.cpp \
    timingdialog/timingitemwidget.cpp \
    timingdialog/toggleswitch.cpp \
    zerodialog/zerodialog.cpp \
    languages/languages.cpp \

HEADERS += \
    mainwindow/messagelist.h \
    mainwindow/messagewidget.h \
    messages.h \
    requests/dumpgetrequest.h \
    requests/request.h \
    requests/requestqueue.h \
    requests/setpostrequest.h \
    requests/statusgetrequest.h \
    requests/timingpostrequest.h \
    requests/zeropostrequest.h \
    setdialog/setdialog.h \
    timingdialog/dayswitch.h \
    timingdialog/dayswitches.h \
    mainwindow/mainwindow.h \
    timing.h \
    timingdialog/timingdialog.h \
    timingdialog/timingitemwidget.h \
    json.hpp \
    httplib.h \
    timingdialog/toggleswitch.h \
    zerodialog/zerodialog.h \
    languages/languages.h \

FORMS += \
    mainwindow.ui

DEFINES += SERVER_URI='\\"https://your.uri\\"' \
    CPPHTTPLIB_OPENSSL_SUPPORT
    USER_NAME='\\"USER_NAME\\"' \
    PASSWORD='\\"PASSWORD\\"'
	
OPEN_SSL_PATH=C:/Path/To/OpenSSL

LIBS += -lws2_32 \
    -L$$quote("$${OPEN_SSL_PATH}/lib") -llibssl \
    -L$$quote("$${OPEN_SSL_PATH}/lib") -llibcrypto

INCLUDEPATH +=  $$quote("$${OPEN_SSL_PATH}/include")

RESOURCES = application.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
