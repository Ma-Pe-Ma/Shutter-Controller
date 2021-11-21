# Shutter Controller - ESP8266 server
Before reading this through make sure you have read the [project's ReadMe](/ReadMe.md).

Important note: the ESP reboots after every 48 days to prevent time overflowing.
## Setting up
### Core
The ESP is programmed in Arduno environment. For this you have to install the proper [core](https://github.com/esp8266/Arduino) (installation instructions are also included).
### Library dependencies
These libraries can be installed from the Arduino library manager:
 * [EasyDDNS](https://github.com/ayushsharma82/EasyDDNS)
 * [ArduinoQueue](https://github.com/EinarArnason/ArduinoQueue)
 * [ArduinoJSON](https://github.com/bblanchon/ArduinoJson)
 * [NTP Client](https://github.com/arduino-libraries/NTPClient)
 
At the time of writing this document, in the NTP client's latest release (v3.2.0) the update function always returns true not just only when the update happened. Later commits fix this, but there hasn't been a release with this fix. If there is now a newer release you can ignore the instructions below.

To manually fix this, open 

    %Path_To_Your_Arduino_Libraries/NTPClient/NTPClient.cpp
and edit line 101 to

    return false;
(Note: [this is the file](https://github.com/arduino-libraries/NTPClient/blob/151bbc5484265cbeeb0273a3752a95aedda22f2f/NTPClient.cpp) in question which should be edited locally.)

### Configuring
The server can be configured in the configuration.h and configuration.cpp files.
* SSID and PSWD: the SSID and the password of the local WiFi network to which the ESP connects
* DDNS properties: checkout [EasyDDNS's readme](https://github.com/ayushsharma82/EasyDDNS#readme) what they mean
* User-name and password: simple authentications for requests
* Key and Certificate files: you have to set the openssl created values  the .cpp file
* Default page: simple html source for the default page
* Up and down pins
* Total time of the shutter's operations of going up and down

## Back Log
* Shutter speed
* implement configuring automatic zeroing at given time? (+clients too)