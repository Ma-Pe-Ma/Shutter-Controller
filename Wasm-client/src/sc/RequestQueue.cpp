#include "RequestQueue.h"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include "EmscriptenGlue.h"
#endif 

bool RequestQueue::isActive()
{
	std::lock_guard<std::mutex> guard(requestMutex);
	return !requestQueue.empty() || currentRequest != nullptr;
}

void RequestQueue::pushNewRequest(std::shared_ptr<Request> newRequest)
{
	std::lock_guard<std::mutex> guard(requestMutex);
	requestQueue.push(newRequest);
}

void RequestQueue::pushNewRequestIfQueueIsEmpty(std::shared_ptr<Request> newRequest)
{
	std::lock_guard<std::mutex> guard(requestMutex);

	if (requestQueue.size() == 0 && currentRequest == nullptr)
	{
		requestQueue.push(newRequest);		
	}
}

void RequestQueue::tryDequeue()
{
	std::lock_guard<std::mutex> guard(requestMutex);

	// handle finished request
	if (currentRequest != nullptr && currentRequest->isFinished())
	{
		lastRequestSuccess = currentRequest->lastRequestSuccess;

		std::shared_ptr<Request> newRequest = currentRequest->callback != nullptr ? currentRequest->callback(currentRequest->response) : nullptr;

		if (newRequest != nullptr)
		{
			requestQueue.push(newRequest);
		}

		currentRequest->requestThread.join();

		currentRequest = nullptr;
	}
	// dequeue new request
	else if (currentRequest == nullptr && requestQueue.size() > 0)
	{
		currentRequest = requestQueue.front();
		requestQueue.pop();

		currentRequest->setSiteAddress(siteAddress);
		currentRequest->setParameters(parameters);

		currentRequest->launchRequest();
	}
}

void RequestQueue::terminate()
{
	if (currentRequest != nullptr)
	{
		currentRequest->tryDetach();
	}

	requestQueue = std::queue<std::shared_ptr<Request>>();
}