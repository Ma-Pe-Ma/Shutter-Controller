#ifndef ZEROPOSTREQUEST_H
#define ZEROPOSTREQUEST_H

#include <QThread>
#include <QApplication>

#include "request.h"

#include "json.hpp"
#include "messages.h"

using json = nlohmann::json;

enum Zero {
    down,
    up,
    automatic
};

class ZeroPostRequest : public Request {
    Q_OBJECT
private:

public:
    ZeroPostRequest(Zero);
    void run() override;
};

#endif // ZEROPOSTREQUEST_H
