#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Arduino.h"

//wireless network properties
#define SSID ""
#define PSWD ""

//ports on which the server listens
#define HTTP_PORT 80
#define HTTPS_PORT 443

//DDNS properties
#define DDNS_SERVICE ""
#define DDNS_DOMAIN ""
#define DDNS_TOKEN ""

//simple authentication values
#define USER_NAME ""
#define PASSWORD ""

//the html page served at the root
#define DEFAULT_PAGE "<!DOCTYPE html><html><head></head><body><form action=\"./L\" method=\"post\">" \
	"<label for=\"fname\">Usarname:</label><br>" \
	"<input type=\"text\" name=\"U\" value=\"\"><br><br>" \
	"<label for=\"fname\">Password:</label><br>" \
	"<input type=\"password\" name=\"P\" value=\"\"><br><br>" \
	"<input type=\"submit\" value=\"Login\">" \
"</form></body></html>"

#define CONTROL_PAGE "<!DOCTYPE html><html><head></head><body>"\
    "<script>"\
        "var s = new XMLHttpRequest();"\
        "s.onload=function() {"\
            "console.log(\"Testing\");"\
        "}"\
        "var i = setInterval(function() {"\
            "s.open(\"GET\", \"/S\");"\
            "s.send();"\
        "}, 30000);"\
    "</script>"\
    "TESTING"\
"</body></html>"

//unfortunately I can't define a multiline raw string macro properly
//so these variables are needed
extern const char serverCert[] PROGMEM;
extern const char serverKey[] PROGMEM;

//the up and dow pin definitions
#define UP_PIN 0
#define DOWN_PIN 2

//not too high amount of timings and processes
#define NR_OF_TIMINGS 6
#define NR_OF_PROCESSES 6

//working time of shutter in secs in both ways
#define UP_TIME 27.0f
#define DOWN_TIME 26.0f

//easy way to swap the working logic of up/down relays
#define ACTIVATE_PIN LOW
#define DEACTIVATE_PIN HIGH

#endif