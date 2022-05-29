#include "setpostrequest.h"
#include <QDebug>
#include <QApplication>

SetPostRequest::SetPostRequest(int value) {
    timeout = 10;
    location = "/V";

    json valueObject;
    valueObject["V"] = value;

    postString = valueObject.dump();
}

void SetPostRequest::run() {
    Request::post();
}
