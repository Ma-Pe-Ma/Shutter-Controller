#ifndef REQUESTQUEUE_H
#define REQUESTQUEUE_H

#include <queue>
#include <memory>
#include <mutex>
#include <atomic>

#include "request/Request.h"

class RequestQueue {
	std::queue<std::shared_ptr<Request>> requestQueue;
	std::shared_ptr<Request> currentRequest = nullptr;
	std::mutex requestMutex;

	std::string siteAddress = "";
	std::string parameters = "";

	bool lastRequestSuccess = true;
public:
	void setSiteAddress(std::string siteAddress, std::string parameters) {
#if !defined CPPHTTPLIB_OPENSSL_SUPPORT && !defined __EMSCRIPTEN__
		if (siteAddress.substr(0, 5) == "https") {
			siteAddress = "http" + siteAddress.substr(5);
		}
#endif

		this->siteAddress = siteAddress;
		this->parameters = parameters;
	}

	void pushNewRequest(std::shared_ptr<Request> newRequest);
	void pushNewRequestIfQueueIsEmpty(std::shared_ptr<Request> newRequest);	

	bool isActive();
	bool getLastRequestSuccess() { return lastRequestSuccess; }	

	void tryDequeue();
	void terminate();
};

#endif // !REQUESTQUEUE_H
