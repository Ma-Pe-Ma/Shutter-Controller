#ifndef REQUESTQUEUE_H
#define REQUESTQUEUE_H

#include <json.hpp>
#include <QObject>
#include <QQueue>

#include <mutex>

class Request;

class RequestQueue : public QObject {
  Q_OBJECT
    Request* currentRequest;
    QQueue<Request*> queue;
    std::mutex queueMutex;

public:
    RequestQueue();

    void enqueueRequest(Request*);

signals:
    void notifyMessage(std::string);

public slots:
    void notifyRequestEnd(std::string);
    void isEmpty(bool&);
};

#endif // REQUESTQUEUE_H
