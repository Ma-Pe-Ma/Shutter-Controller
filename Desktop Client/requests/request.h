#ifndef REQUEST_H
#define REQUEST_H

#include <QThread>
#include <QApplication>
#include "httplib.h"

class RequestQueue;

class Request : public QThread {
    Q_OBJECT

public:
    static RequestQueue requestQueue;
    Request();
    virtual void run() override;

protected:
    const QString qPath = QCoreApplication::applicationDirPath() + "/cert.pem";
    const std::string path = qPath.toUtf8().constData();
    const std::string arguments = "?username=" + std::string(USER_NAME) +"&password=" + std::string(PASSWORD);
    std::string location = "";
    unsigned int timeout = 10000;
    std::string postString = "";

    void get();
    void post();
signals:
    void customSignal();
    void requestFinish(std::string);
};

#endif // REQUEST_H
