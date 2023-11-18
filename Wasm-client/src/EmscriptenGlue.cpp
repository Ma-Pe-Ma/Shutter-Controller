#include "EmscriptenGlue.h"

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/html5.h>

EM_JS(int, canvas_get_width, (), {
	return canvas.getBoundingClientRect().width;
	});

EM_JS(int, canvas_get_height, (), {
	return canvas.getBoundingClientRect().height;
	});

EM_JS(int, screen_get_width, (), {
	return screen.width;
	});

EM_JS(int, screen_get_height, (), {
	return screen.height;
	});

EM_JS(int, window_get_width, (), {
	return window.width;
	});

EM_JS(int, window_get_height, (), {
	return window.height;
	});

EM_JS(bool, is_mobile, (), {
	var a = false;

	if (navigator.userAgent.match(/Android/i)
		 || navigator.userAgent.match(/webOS/i)
		 || navigator.userAgent.match(/iPhone/i)
		 || navigator.userAgent.match(/iPad/i)
		 || navigator.userAgent.match(/iPod/i)
		 || navigator.userAgent.match(/BlackBerry/i)
		 || navigator.userAgent.match(/Windows Phone/i)
		 ) {
			a = true;
	}

	return a;
	});

EM_JS(void, setup_broadcast_channel, (), {
	const bc = new BroadcastChannel("redirectChannel");
	bc.onmessage = (event) => {
			if (event.data.hasOwnProperty("redirect"))
			{
				window.location.href = event.data["redirect"];
			}
		};
	});

EM_JS(void, redirect_to_location, (const char* path), {
	var redirectMessage = {
		"redirect" : UTF8ToString(path)
	};

	const bc = new BroadcastChannel("redirectChannel");
	bc.postMessage(redirectMessage);
	bc.close();
	});

EM_JS(char*, create_datestring_by_offset, (int hour, int minute), {
	var date = new Date(Date.now() - 1000 * 60 * (hour * 60 + minute));
	var h = date.getHours() < 10 ? '0' + date.getHours() : date.getHours();
	var m = date.getMinutes() < 10 ? '0' + date.getMinutes() : date.getMinutes();

	return stringToNewUTF8(`${date.getFullYear()}. ${ date.getMonth() + 1 }. ${ date.getDate() }. ${ h }:${ m }`);
	});

EM_JS(int, get_today, (), {
		var date = new Date();
		let today = date.getDay();
		return today > 0 ? today - 1 : 6;
	});

EM_JS(void, timeout_method, (int index, int timingMinutes), {
	setTimeout(function(index, originalMinutes) { 
		Module.DemoRequest.launchTiming(index, originalMinutes);
	}, (timingMinutes - get_sum_minutes()) * 60 * 1000, index, timingMinutes);
	});

EM_JS(int, get_sum_minutes, (), {
		var date = new Date();
		return 60 * date.getHours() + date.getMinutes();
	});

EM_JS(bool, use_demo_request, (), {
	if (Module['demoMode'] == null) {
		return false;
	}

	return Module['demoMode'];
});

#endif