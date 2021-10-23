#ifndef TIMINGPOSTREQUEST_H
#define TIMINGPOSTREQUEST_H

#include "request.h"
#include "json.hpp"
#include "messages.h"

using json = nlohmann::json;

class TimingPostRequest : public Request {
    Q_OBJECT
private:

public:
    TimingPostRequest(json);
    void run() override;
};
#endif // TIMINGPOSTREQUEST_H
