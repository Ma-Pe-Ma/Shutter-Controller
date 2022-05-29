#ifndef STATUSGETREQUEST_H
#define STATUSGETREQUEST_H

#include "request.h"
#include <json.hpp>
#include <QApplication>

#include "json.hpp"
#include "messages.h"

using json = nlohmann::json;

class StatusGetRequest : public Request {
    Q_OBJECT
public:
    StatusGetRequest();
};

#endif // STATUSGETREQUEST_H
