#ifndef DEMOREQUEST_H
#define DEMOREQUEST_H

#include <thread>
#include <chrono>

#include <json.hpp>
#include <emscripten/bind.h>

#include "../EmscriptenGlue.h"
#include "Request.h"

class DemoRequest : public Request {
	inline static nlohmann::json dummyMessages = {
		{"0", {{"T", "S"}, {"R", "M"}, {"A", "45"}, {"D", {0, 17}} }},
		{"1", {{"T", "Z"}, {"R", "O"}, {"A", "U"}, {"D",{3, 4}} }},
		{"2", {{"T", "S"}, {"R", "T"}, {"A", ""}, {"D", {4, 37}} }},
		{"3", {{"T", "T"}, {"R", "1"}, {"A", "70"}, {"D", {11, 11}} }},
		{"4", {{"T", "S"}, {"R", "M"}, {"A", "30"}, {"D", {23, 2}} }},
		{"5", {{"T", "I"}, {"R", ""}, {"A", ""}, {"D", {23, 5}} }},
		{"6", {{"T", "S"}, {"R", "T"}, {"A", ""}, {"D", {23, 28}} }},
		{"7", {{"T", "S"}, {"R", "M"}, {"A", "20"}, {"D", {23, 50}} }},
		{"8", {{"T", "S"}, {"R", "Z"}, {"A", "15"}, {"D", {25, 3}} }},
		{"9", {{"T", "S"}, {"R", "M"}, {"A", "15"}, {"D", {26, 18}} }}
	};

	inline static nlohmann::json dummyTimings = {
		{"0" , {{"H" ,  6}, {"M" , 0}, {"D", "TTTTTFF"}, {"A",  true}, {"V", 100}}},
		{"1" , {{"H" , 18}, {"M" , 0}, {"D", "TTTTTFF"}, {"A",  true}, {"V",   0}}},
		{"2" , {{"H" ,  8}, {"M" , 0}, {"D", "FFFFFTT"}, {"A",  true}, {"V", 100}}},
		{"3" , {{"H" , 22}, {"M" , 0}, {"D", "FFFFFTT"}, {"A",  true}, {"V",  0}}},
		{"4" , {{"H" , 15}, {"M" , 30}, {"D", "FFFFFTT"}, {"A", false}, {"V",  50}}},
		{"5" , {{"H" , 12}, {"M" , 45}, {"D", "TTTTTFF"}, {"A", false}, {"V",  50}}}
	};

	inline static int dummyValue = 45;
	inline static std::mutex messageMutex;

	inline static void addNewMessage(std::string t, std::string r, std::string a) {
		std::lock_guard<std::mutex> lock(messageMutex);

		for (int i = 8; i >= 0; i--) {
			dummyMessages[std::to_string(i + 1)] = dummyMessages[std::to_string(i)];
		}

		dummyMessages["0"] = { {"T", t}, {"R", r}, {"A", a}, {"D", std::string(create_datestring_by_offset(0, 0))} };
	}

	inline static void dummyTimingSetter() {
		int today = get_today();
		int sumMinutes = get_sum_minutes();

		for (int i = 0; i < 6; i++) {
			nlohmann::json timing = dummyTimings[std::to_string(i)];

			if (timing["D"].get<std::string>()[today] == 'T' && timing["A"].get<bool>()) {
				int timingMinutes = 60 * timing["H"].get<int>() + timing["M"].get<int>();				

				if (sumMinutes < timingMinutes && timingMinutes < sumMinutes + 30) {
					timeout_method(i, timingMinutes);
				}
			}
		}
	}

	void launchRequest() override {
		std::this_thread::sleep_for(std::chrono::milliseconds(this->delay * 1000));

		requestThread = std::thread([this]() -> void {
			int delay = 0;
			
			if (location == "/D") {
				delay = 1;
			}
			else if (location == "/V") {
				int newValue = (*postdata)["V"].get<int>();
				
				if (dummyValue != newValue) {				
					dummyValue = newValue;
					addNewMessage("S", "M", std::to_string(dummyValue));				
					delay = 3;
				}
			}
			else if (location == "/T") {
				dummyTimings = (*postdata);
				addNewMessage("S", "T", "");
				dummyTimingSetter();
			}
			else if (location == "/Z") {
				std::string z = (*postdata)["Z"].get<std::string>();

				if (z == "up") {
					delay = 2;
					addNewMessage("Z", "O", "U");
					dummyValue = 100;
				}
				else if (z == "down") {
					delay = 2;
					addNewMessage("Z", "O", "D");
					dummyValue = 0;
				}
				else {
					delay = 10;
					addNewMessage("S", "Z", std::to_string(dummyValue));
				}
			}

			this->response = {
				{"G",	{
						{"R", 0},
						{"V" , DemoRequest::dummyValue},
						{"M" ,	{
									{"M", DemoRequest::dummyMessages},
									{"S", std::string(create_datestring_by_offset(23, 5))},
								}
						}
						}
				}
			};

			if (location == "/D") {
				this->response["T"] = DemoRequest::dummyTimings;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(delay * 1000));
			finished.store(true);
		});
	}

public:
	inline static void initialize() {
		for (int i = 0; i < 10; i++) {
			nlohmann::json& message = dummyMessages[std::to_string(i)];
			message["D"] = std::string(create_datestring_by_offset(message["D"][0].get<int>(), message["D"][1].get<int>()));
		}

		dummyTimingSetter();
	}	

	inline static void launchTiming(int index, int originalMinutes) {
		nlohmann::json& timing = dummyTimings[std::to_string(index)];

		int timingMinutes = timing["H"].get<int>() * 60 + timing["M"].get<int>();

		if (abs(timingMinutes - originalMinutes) <= 1) {
			dummyValue = timing["V"].get<int>();
			addNewMessage("T", std::to_string(index + 1), std::to_string(dummyValue));
		}
	}
};

// Binding code
EMSCRIPTEN_BINDINGS(DemoRequestBind) {
	emscripten::class_<DemoRequest>("DemoRequest")
		.class_function("launchTiming", &DemoRequest::launchTiming)
		;
}

#endif