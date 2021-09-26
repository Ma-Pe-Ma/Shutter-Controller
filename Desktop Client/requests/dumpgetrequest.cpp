#include "dumpgetrequest.h"
#include <QDebug>
//#include "httplib.h"

DumpGetRequest::DumpGetRequest() {
    connect(this, &DumpGetRequest::processResponseSignal, this, &DumpGetRequest::processResponse);
}

void DumpGetRequest::run() {
    QString qPath = QCoreApplication::applicationDirPath() +"/cert.pem";
    std::string path = qPath.toUtf8().constData();

    httplib::Client cli(SERVER_URI);
    cli.set_ca_cert_path(path.c_str());
    cli.enable_server_certificate_verification(true);
    cli.set_connection_timeout(5000);

    httplib::Result res = cli.Get("/D");

    if (res == nullptr) {
        emit dumpGetEnd(false);
        //qInfo()<<"Dump res null!";
    }
    else {
        emit processResponseSignal(res->body);
    }
}

void DumpGetRequest::processResponse(std::string dumpString) {
    json dumpObject = json::parse(dumpString);

    json timingObject = dumpObject["T"].get<json>();
    json genericResponse = dumpObject["G"].get<json>();
    
    Timing::parseTimings(timingObject);
    Messages::parseGenericResponse(genericResponse);

    emit dumpGetEnd(true);
}
