#include "timingpostrequest.h"
#include <QDebug>

TimingPostRequest::TimingPostRequest(json timingObject) {
    sendableTimingObject = timingObject;
    connect(this, &TimingPostRequest::processResponseSignal, this, &TimingPostRequest::processResponse);
}

void TimingPostRequest::run() {
    //qInfo()<<"Timing Post Request Started!";

    QString qPath = QCoreApplication::applicationDirPath() +"/cert.pem";
    std::string path = qPath.toUtf8().constData();

    httplib::Client cli(SERVER_URI);
    cli.set_ca_cert_path(path.c_str());
    cli.enable_server_certificate_verification(true);
    cli.set_connection_timeout(10000);

    std::string timingString = sendableTimingObject.dump();

    httplib::Result res = cli.Post("/T", timingString.c_str(), "text/plain");

    if (res == nullptr) {
        emit timingPostEnd(false);
    }
    else {
        emit processResponseSignal(res->body);
    }
}

void TimingPostRequest::processResponse(std::string response) {
    json responseObject = json::parse(response);
    Messages::parseGenericResponse(responseObject);

    //qInfo()<<"timing post ended0!";

    emit timingPostEnd(true);
}


