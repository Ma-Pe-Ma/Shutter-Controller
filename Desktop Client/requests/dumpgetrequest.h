#ifndef DUMPGETREQUEST_H
#define DUMPGETREQUEST_H

#include "request.h"
#include <json.hpp>
#include "timing.h"
#include "messages.h"

using json = nlohmann::json;

class DumpGetRequest : public Request {
    Q_OBJECT
public:
    DumpGetRequest();
};
#endif // REQUESTDISPATCHER_H
