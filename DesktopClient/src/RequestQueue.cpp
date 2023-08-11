#include "RequestQueue.h"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include "EmscriptenGlue.h"
#endif 

bool RequestQueue::isActive()
{
	bool isActive = false;

	requestMutex.lock();

	isActive = !requestQueue.empty() || currentRequest != nullptr;

	requestMutex.unlock();

	return isActive;
}

void RequestQueue::pushNewRequest(std::shared_ptr<Request> newRequest)
{
	requestMutex.lock();
	requestQueue.push(newRequest);
	requestMutex.unlock();
}

void RequestQueue::pushNewRequestIfQueueIsEmpty(std::shared_ptr<Request> newRequest)
{
	requestMutex.lock();

	if (requestQueue.size() == 0 && currentRequest == nullptr)
	{
		requestQueue.push(newRequest);		
	}

	requestMutex.unlock();
}

void RequestQueue::tryDequeue()
{
	requestMutex.lock();

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
	
	requestMutex.unlock();	
}

void RequestQueue::terminate()
{
	if (currentRequest != nullptr)
	{
		currentRequest->tryDetach();
	}

	requestQueue = std::queue<std::shared_ptr<Request>>();
}