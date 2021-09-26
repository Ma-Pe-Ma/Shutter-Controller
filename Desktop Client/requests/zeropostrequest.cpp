#include "zeropostrequest.h"

ZeroPostRequest::ZeroPostRequest(Zero zero) {
    zeroString = "AUT";

    switch(zero) {
        case automatic:
            zeroString = "find";
            break;
        case down:
            zeroString = "down";
            break;
        case up:
            zeroString = "up";
            break;
    }

    connect(this, &ZeroPostRequest::processResponseSignal, this, &ZeroPostRequest::processResponse);
}

void ZeroPostRequest::run() {
    QString qPath = QCoreApplication::applicationDirPath() +"/cert.pem";
    std::string path = qPath.toUtf8().constData();

    httplib::Client cli(SERVER_URI);
    cli.set_ca_cert_path(path.c_str());
    cli.enable_server_certificate_verification(true);
    cli.set_connection_timeout(10000);

    json zeroObject;
    zeroObject["Z"] = zeroString;

    httplib::Result res = cli.Post("/Z", zeroObject.dump(), "text/plain");

    if (res == nullptr) {
        emit zeroPostEnd(false);
    }
    else {
        emit processResponseSignal(res->body);
    }
}

void ZeroPostRequest::processResponse(std::string response) {
    json responseObject = json::parse(response);
    Messages::parseGenericResponse(responseObject);

    emit zeroPostEnd(true);
}
