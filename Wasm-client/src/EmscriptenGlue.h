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
}	

#endif

#endif