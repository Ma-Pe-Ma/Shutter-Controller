#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Arduino.h"

//format the persistent memory
#define FORMAT_FS_ONSTARTUP false

//disable authentication during development
//#define DISABLE_AUTHENTICATION

//wireless network properties
#define SSID ""
#define PSWD ""

//location where the html and wasm resource files are served
#define RESOURCE_PROVIDER "https://path.to/resources"

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

//the up and dow pin definitions
#define UP_PIN 0
#define DOWN_PIN 2

//not too high amount of timings, processes and messages
#define NR_OF_TIMINGS 6
#define NR_OF_PROCESSES 6
#define NR_OF_MESSAGES 10

//working time of shutter in secs in both ways
#define UP_TIME 27.0f
#define DOWN_TIME 26.0f

//easy way to swap the working logic of up/down relays
#define ACTIVATE_PIN LOW
#define DEACTIVATE_PIN HIGH

#endif