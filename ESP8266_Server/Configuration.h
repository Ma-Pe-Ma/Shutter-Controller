#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Arduino.h"

#define SSID ""
#define PSWD ""

#define HTTP_PORT 80
#define HTTPS_PORT 443

#define DDNS_SERVICE ""
#define DDNS_DOMAIN ""
#define DDNS_TOKEN ""

#define USER_NAME ""
#define PASSWORD ""

#define DEFAULT_PAGE ""

//unfortunately I can't define a raw string macro properly
//so these variables are needed
extern const char serverCert[] PROGMEM;
extern const char serverKey[] PROGMEM;

#define UP_PIN 0
#define DOWN_PIN 2

#define NR_OF_TIMINGS 6
#define UP_SPEED 0.1f
#define DOWN_SPEED 0.1f

#endif