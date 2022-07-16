#include "requestqueue.h"
#include "request.h"
#include <QDebug>

RequestQueue::RequestQueue() {

}

void RequestQueue::enqueueRequest(Request* newRequest) {
    queueMutex.lock();

    if (this->currentRequest == nullptr) {
        this->currentRequest = newRequest;
        connect(this->currentRequest, &Request::requestFinish, this, &RequestQueue::notifyRequestEnd);
        this->currentRequest->start();
    }
    else {
        queue.enqueue(newRequest);
    }

    queueMutex.unlock();
}

void RequestQueue::notifyRequestEnd(std::string message) {
    emit notifyMessage(message);

    queueMutex.lock();

    emit currentRequest->customSignal();

    currentRequest->disconnect();

    while(currentRequest->isRunning());
    delete currentRequest;
    currentRequest = nullptr;

    if (!queue.isEmpty()) {
        currentRequest = queue.dequeue();
        connect(this->currentRequest, &Request::requestFinish, this, &RequestQueue::notifyRequestEnd);
        currentRequest->start();
    }

    queueMutex.unlock();
}

void RequestQueue::isEmpty(bool& empty) {
    queueMutex.lock();
    empty = queue.isEmpty() && (this->currentRequest == nullptr);
    queueMutex.unlock();
}
