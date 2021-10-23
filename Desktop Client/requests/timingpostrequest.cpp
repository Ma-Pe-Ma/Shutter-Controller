#include "timingpostrequest.h"
#include <QDebug>
#include <QTime>

TimingPostRequest::TimingPostRequest(json timingObject) {
    timeout = 10000;
    location = "/T";

    postString = timingObject.dump();
}

void TimingPostRequest::run() {
    Request::post();
}
