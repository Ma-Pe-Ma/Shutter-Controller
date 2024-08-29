#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <functional>
#include <chrono>
#include <mutex>

#include "RequestQueue.h"
#include "Message.h"

#include "Timing.h"

#include "Shutter.pb.h"

const int numberOfTimings = 6;
const int numberOfMessages = 10;

class MainWindow {
	int windowSize[2] = {};

	int currentValue = 0;
	int sliderValue = 0;
	Timing timings[numberOfTimings];

    std::string startupMessage;
	Message messages[numberOfMessages];

	RequestQueue requestQueue;

	void handleSetting();
	void handleTimings();
	void handleZeroing();
	void handleMessages();

    int refreshPeriod = 30;
    std::chrono::system_clock::time_point lastStatusCheck = std::chrono::system_clock::now();

	std::shared_ptr<std::map<std::string, std::string>> translation;

	std::mutex guiMutex;

	std::shared_ptr<Request> createRequest();

	bool lastRequestOk = false;

	std::vector<std::string> dayNameMap;
public:
	void initializeOfflineRequests(std::shared_ptr<std::map<std::string, std::string>>);
	void initializeRequests();
	void update();

    std::function<std::shared_ptr<Request>(std::vector<unsigned char>)> receiveResponse;

	void terminate();

	void setWindowSize(int x, int y) {
		this->windowSize[0] = x;
		this->windowSize[1] = y;
	}

	void setTranslation(std::shared_ptr<std::map<std::string, std::string>> translation)
	{
		this->translation = translation;
		Message::setTranslation(translation);

		for (int i = 0; i < 7; i++)
		{
			std::string key = "w" + std::to_string(i);
			dayNameMap.push_back((*translation)[key]);
		}
	}
};

#endif 