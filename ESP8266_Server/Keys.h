#ifndef KEYS_H
#define KEYS_H

#include <Arduino.h>

const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";

const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN PRIVATE KEY-----

-----END PRIVATE KEY-----
)EOF";

#endif