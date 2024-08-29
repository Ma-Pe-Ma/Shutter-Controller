#ifndef REQUEST_H
#define REQUEST_H

#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

class Request {

protected:
	std::shared_ptr<std::vector<unsigned char>> postdata = nullptr;
	std::string siteAddress;
	std::string location;
	std::string parameters;
	int timeout = 10;
	int delay = 0;
	std::function<std::shared_ptr<Request>(std::vector<unsigned char>&)> callback = nullptr;

	std::vector<unsigned char> response;

	bool lastRequestSuccess = true;

	std::thread requestThread;
	std::atomic<bool> finished = std::atomic<bool>(false);
public:
	Request() {}	
	void setLocation(std::string location) { this->location = location; }
	void setPostData(std::shared_ptr<std::vector<unsigned char>> postdata) { this->postdata = postdata; }
	void setTimeout(int timeout) { this->timeout = timeout; }
	void setCallback(std::function<std::shared_ptr<Request>(std::vector<unsigned char>&)> callback) { this->callback = callback; }
	void setDelay(int delay) { this->delay = delay; }
	void setSiteAddress(std::string siteAddress) { this->siteAddress = siteAddress; }
	void setParameters(std::string parameters) { this->parameters = parameters; }

	bool isFinished() { return this->finished.load(); }

	virtual void launchRequest() = 0;

	void tryDetach()
	{
		if (requestThread.joinable())
		{
			requestThread.detach();
		}
	}

	friend class RequestQueue;
};

#endif