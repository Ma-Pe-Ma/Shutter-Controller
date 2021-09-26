#include "statusgetrequest.h"
#include <QDebug>
#include <QCoreApplication>

StatusGetRequest::StatusGetRequest() {
    connect(this, &StatusGetRequest::processResponseSignal, this, &StatusGetRequest::processResponse);
}

void StatusGetRequest::run() {
    QString qPath = QCoreApplication::applicationDirPath() +"/cert.pem";
    std::string path = qPath.toUtf8().constData();

    httplib::Client cli(SERVER_URI);
    cli.set_ca_cert_path(path.c_str());
    cli.enable_server_certificate_verification(true);
    cli.set_connection_timeout(10000);

    httplib::Result res = cli.Get("/S");

    if (res == nullptr) {
        emit statusGetEnd(false);
    }
    else {
        //qInfo()<<"BODY: "<<res->body.c_str();
        //qInfo()<<"ERROR CODE: "<<QString::number(res.error());
        emit processResponseSignal(res->body);
    }

    //emit statusReceived("");
}

void StatusGetRequest::processResponse(std::string response) {
    json responseObject = json::parse(response);
    Messages::parseGenericResponse(responseObject);

    emit statusGetEnd(true);
}
