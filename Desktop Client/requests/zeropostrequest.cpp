#include "zeropostrequest.h"
#include <QDebug>

ZeroPostRequest::ZeroPostRequest(Zero zero) {
    timeout = 10000;
    location = "/Z";

    std::string zeroString = "AUT";

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

    json zeroObject;
    zeroObject["Z"] = zeroString;

    postString = zeroObject.dump();
}

void ZeroPostRequest::run() {
    Request::post();
}
