//if I could properly define a multiline raw string macro
//then this file could be removed
#include "Configuration.h"

const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";

const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN PRIVATE KEY-----

-----END PRIVATE KEY-----
)EOF";