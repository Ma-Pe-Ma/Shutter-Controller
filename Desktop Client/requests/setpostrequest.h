#ifndef SETPOSTREQUEST_H
#define SETPOSTREQUEST_H

#include <QThread>
#include "httplib.h"
#include <QApplication>
#include "json.hpp"
#include "messages.h"

using json = nlohmann::json;

class SetPostRequest : public QThread {
    Q_OBJECT
public:
    SetPostRequest(int);
    int value = 0;

    void run() override;
signals:
    void setPostEnd(bool);
    void processResponseSignal(std::string);

private slots:
    void processResponse(std::string);
};

#endif // SETPOSTREQUEST_H
