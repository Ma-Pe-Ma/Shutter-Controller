#ifndef EMSCRIPTENGLUE_H
#define EMSCRIPTENGLUE_H

#ifdef __EMSCRIPTEN__

extern "C" {
	int canvas_get_width();
	int canvas_get_height();
	int screen_get_width();
	int screen_get_height();
	int window_get_width();
	int window_get_height();
	bool is_mobile();
	void setup_broadcast_channel();
	void redirect_to_location(const char* location);	
	
	char* create_datestring_by_offset(int hour, int minute);
	int get_today();
	int get_sum_minutes();
	void timeout_method(int, int);
	bool use_demo_request();
}

#endif

#endif