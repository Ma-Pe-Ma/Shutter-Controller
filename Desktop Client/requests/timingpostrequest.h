#ifndef TIMINGPOSTREQUEST_H
#define TIMINGPOSTREQUEST_H

#include <QThread>
#include "httplib.h"
#include "json.hpp"
#include <QApplication>
#include "messages.h"

using json = nlohmann::json;

class TimingPostRequest : public QThread {
    Q_OBJECT
private:
    json sendableTimingObject;

public:
    TimingPostRequest(json);
    void run() override;

signals:
    void processResponseSignal(std::string);
    void timingPostEnd(bool);
private slots:
    void processResponse(std::string);
};
#endif // TIMINGPOSTREQUEST_H
