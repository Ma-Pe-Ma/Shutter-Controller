#ifndef STATUSGETREQUEST_H
#define STATUSGETREQUEST_H

#include "httplib.h"
#include <QThread>
#include <json.hpp>
#include <QApplication>

#include "json.hpp"
#include "messages.h"

using json = nlohmann::json;

class StatusGetRequest : public QThread {
    Q_OBJECT
public:
    StatusGetRequest();
    void run() override;
signals:
    void processResponseSignal(std::string);
    void statusGetEnd(bool);

public slots:
    void processResponse(std::string);
};

#endif // STATUSGETREQUEST_H
