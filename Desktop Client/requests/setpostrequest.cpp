#include "setpostrequest.h"
#include <QDebug>
#include <QApplication>

SetPostRequest::SetPostRequest(int value) {
    this->value = value;
    connect(this, &SetPostRequest::processResponseSignal, this, &SetPostRequest::processResponse);
}

void SetPostRequest::run() {
    QString qPath = QCoreApplication::applicationDirPath() +"/cert.pem";
    std::string path = qPath.toUtf8().constData();

    httplib::Client cli(SERVER_URI);
    cli.set_ca_cert_path(path.c_str());
    cli.enable_server_certificate_verification(true);
    cli.set_connection_timeout(10000);

    json valueObject;
    valueObject["V"] = this->value;

    //qInfo()<<"Sent value: "<<this->value;

    httplib::Result res = cli.Post("/V", valueObject.dump(), "text/plain");

    if (res == nullptr) {
        emit setPostEnd(false);
    }
    else {
        emit processResponseSignal(res->body);
    }
}

void SetPostRequest::processResponse(std::string response) {
    json responseObject = json::parse(response);
    Messages::parseGenericResponse(responseObject);

    emit setPostEnd(true);
}
