#ifndef SETPOSTREQUEST_H
#define SETPOSTREQUEST_H

#include "request.h"
#include "json.hpp"
#include "messages.h"

using json = nlohmann::json;

class SetPostRequest : public Request {
    Q_OBJECT
public:
    SetPostRequest(int);
    void run() override;
};

#endif // SETPOSTREQUEST_H
