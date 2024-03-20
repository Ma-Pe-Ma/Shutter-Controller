#ifndef DEMOREQUEST_H
#define DEMOREQUEST_H

#include <thread>
#include <chrono>

#include <emscripten/bind.h>

#include "../EmscriptenGlue.h"
#include "Request.h"

#include "../Shutter.pb.h"

#include "../Timing.h"
#include "../Message.h"

class DemoRequest : public Request {

	inline static Message dummyMessages[10] = {
		Message(Shutter::Event::set, 45, std::string(create_datestring_by_offset(0, 17))),
		Message(Shutter::Event::zero, 100, std::string(create_datestring_by_offset(3, 4))),
		Message(Shutter::Event::timings_updated, 0, std::string(create_datestring_by_offset(4, 3))),
		Message(Shutter::Event::timing, 70, std::string(create_datestring_by_offset(11, 11))),
		Message(Shutter::Event::set, 30, std::string(create_datestring_by_offset(23, 2))),
		Message(Shutter::Event::start, 0, std::string(create_datestring_by_offset(23, 5))),
		Message(Shutter::Event::timings_updated, 0, std::string(create_datestring_by_offset(23, 28))),
		Message(Shutter::Event::set, 20, std::string(create_datestring_by_offset(23, 50))),
		Message(Shutter::Event::zero_find, 15, std::string(create_datestring_by_offset(25, 3))),
		Message(Shutter::Event::set, 15, std::string(create_datestring_by_offset(26, 18))),
	};

	inline static Timing dummyTiming(int hour, int minute, std::array<bool, 7> days, bool active, int value) {
		Timing timing;
		timing.hour = hour;
		timing.minute = minute;
		timing.active = active;
		timing.value = value;
		std::copy(std::begin(days), std::end(days), timing.days);

		return timing;
	};

	inline static Timing dummyTimings[6]{
		dummyTiming(6, 0, {true, true, true, true, true, false, false}, true, 100),
		dummyTiming(18, 0, {true, true, true, true, true, false, false}, true, 0),
		dummyTiming(8, 0, {false, false, false, false, false, true, true}, true, 100),
		dummyTiming(22, 0, {false, false, false, false, false, true, true}, true, 0),
		dummyTiming(15, 30, {false, false, false, false, false, true, true}, false, 50),
		dummyTiming(12, 45, {true, true, true, true, true, false, false}, false, 50)
	};

	inline static int dummyValue = 45;
	inline static std::string startTime = std::string(create_datestring_by_offset(23, 5));
	inline static std::mutex messageMutex;

	inline static void addNewMessage(Shutter::Event event, int value) {
		std::lock_guard<std::mutex> lock(messageMutex);

		for (int i = 8; i >= 0; i--) {
			dummyMessages[i + 1] = dummyMessages[i];
		}

		dummyMessages[0] = Message(event, value, std::string(create_datestring_by_offset(0, 0)));
	}

	void launchRequest() override {
		std::this_thread::sleep_for(std::chrono::milliseconds(this->delay * 1000));

		requestThread = std::thread([this]() -> void {
			int delay = 0;
			
			if (location == "/D") {
				delay = 1;
			}
			else if (location == "/V") {
				Shutter::Request requestData;
				requestData.ParseFromArray(&(*postdata)[0], postdata->size());

				if (dummyValue != requestData.value()) {
					delay = int(10.0f * abs(dummyValue - requestData.value()) / 100);
					dummyValue = requestData.value();
					addNewMessage(Shutter::Event::set, dummyValue);
				}
			}
			else if (location == "/T") {
				Shutter::TimingContainer timingContainer;
				timingContainer.ParseFromArray(&(*postdata)[0], postdata->size());

				for (int i = 0; i < 6; i++) {
					dummyTimings[i].receiveProtoObject(timingContainer.timing(i));
				}

				delay = 1;

				addNewMessage(Shutter::Event::timings_updated, 0);
				dummyTimingSetter();
			}
			else if (location == "/Z") {
				Shutter::Request requestData;
				requestData.ParseFromArray(&(*postdata)[0], postdata->size());

				if (requestData.zero() == Shutter::Zero::up) {
					delay = 2;
					addNewMessage(Shutter::Event::zero, 100);
					dummyValue = 100;
				}
				else if (requestData.zero() == Shutter::Zero::down) {
					delay = 2;
					addNewMessage(Shutter::Event::zero, 0);
					dummyValue = 0;
				}
				else {
					delay = 10;
					addNewMessage(Shutter::Event::zero_find, dummyValue);
				}
			}

			if (location == "/D") {
				Shutter::TimingContainer timingContainer;

				for (int i = 0; i < 6; i++) {
					DemoRequest::dummyTimings[i].sendProtoObject(timingContainer.add_timing());
				}

				this->response = std::vector<unsigned char>(timingContainer.ByteSizeLong());
				timingContainer.SerializeToArray(this->response.data(), timingContainer.ByteSizeLong());				
			}
			else {
				Shutter::Response responseData;
				responseData.set_value(dummyValue);
				responseData.set_retrytime(delay);

				Shutter::MessageContainer* messageContainer = new Shutter::MessageContainer;

				for (int i = 0; i < 10; i++) {
					Shutter::GenericMessage* newMessage = messageContainer->add_genericmessage();
					Message& message = DemoRequest::dummyMessages[i];
					newMessage->set_datetime(message.date);
					newMessage->set_event(message.event);
					newMessage->set_value(message.value);
				}

				messageContainer->set_starttime(startTime);

				responseData.set_allocated_messagecontainer(messageContainer);

				this->response = std::vector<unsigned char>(responseData.ByteSizeLong());
				responseData.SerializeToArray(this->response.data(), responseData.ByteSizeLong());
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(delay * 1000));
			finished.store(true);
		});
	}

public:
	inline static void dummyTimingSetter() {
		int today = get_today();
		int sumMinutes = get_sum_minutes();

		for (int i = 0; i < 6; i++) {
			Timing timing = dummyTimings[i];

			if (timing.days[today] && timing.active) {
				int timingMinutes = 60 * timing.hour + timing.minute;

				if (sumMinutes < timingMinutes && timingMinutes < sumMinutes + 30) {
					timeout_method(i, timingMinutes);
				}
			}
		}
	}
	inline static void launchTiming(int index, int originalMinutes) {
		Timing& timing = dummyTimings[index];

		int timingMinutes = timing.hour * 60 + timing.minute;

		if (abs(timingMinutes - originalMinutes) <= 1) {
			dummyValue = timing.value;
			addNewMessage(Shutter::Event::timing, dummyValue);
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