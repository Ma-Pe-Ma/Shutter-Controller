#ifndef ZEROPOSTREQUEST_H
#define ZEROPOSTREQUEST_H

#include <QThread>
#include "httplib.h"
#include <QApplication>

#include "json.hpp"
#include "messages.h"

using json = nlohmann::json;

enum Zero {
    down,
    up,
    automatic
};

class ZeroPostRequest : public QThread {
    Q_OBJECT
private:
    std::string zeroString;

public:
    ZeroPostRequest(Zero);
    void run() override;

signals:
    void processResponseSignal(std::string);
    void zeroPostEnd(bool);

private slots:
    void processResponse(std::string);
};

#endif // ZEROPOSTREQUEST_H
