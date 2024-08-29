#ifndef TIMING_H
#define TIMING_H

#include <vector>
#include <string>

#include "Shutter.pb.h"

class Timing {
	int hour = 12;
	int minute = 0;

	bool days[7] = {};
	bool active = false;

	int value = 0;
public:
	void receiveProtoObject(const Shutter::Timing& timing) {
		this->hour = timing.hour();
		this->minute = timing.minute();
		this->active = timing.active();
		this->value = timing.value();

		for (int i = 0; i < timing.days_size(); i++) {
			this->days[i] = timing.days(i);
		}
	}

	void sendProtoObject(Shutter::Timing* timing) {
		timing->set_hour(this->hour);
		timing->set_minute(this->minute);
		timing->set_active(this->active);
		timing->set_value(this->value);		

		for (int i = 0; i < 7; i++) {
			timing->add_days(this->days[i]);
		}
	}

	friend class MainWindow;
	friend class DemoRequest;
};

#endif