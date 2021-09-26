#ifndef DUMPGETREQUEST_H
#define DUMPGETREQUEST_H

#include "httplib.h"
#include <QThread>
#include <json.hpp>
#include "timing.h"
#include "messages.h"
#include <QApplication>

using json = nlohmann::json;

class DumpGetRequest : public QThread {
    Q_OBJECT

public:
    DumpGetRequest();
    void run() override;
signals:
    void processResponseSignal(std::string);
    void dumpGetEnd(bool);
private slots:
    void processResponse(std::string);
};

#ifndef JSON_METATYPE
#define JSON_METATYPE
   Q_DECLARE_METATYPE(json)
#endif

#endif // REQUESTDISPATCHER_H
