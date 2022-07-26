# Shutter Controller - ESP8266 server
Before reading this through make sure you have read the [project's ReadMe](/ReadMe.md).

Important note: the ESP reboots after every 48 days to prevent time overflowing.

## Setting up
### Networking parameters
#### DDNS
Fix IP addresses are generally not available for end-users so this problem is usually bypassed by using a [DDNS service](https://en.wikipedia.org/wiki/Dynamic_DNS). Firstly you have to make sure your connection is not behind [NAT](https://en.wikipedia.org/wiki/Network_address_translation) otherwise you have to ask your internet service provider to solve this (usually this is available to users).

Next you have to register for a DDNS service provider (like No-IP, DynDNS, DuckDNS eg). You will have to choose a domain name and you will get a token which is needed for updating your address. Don't forget to [forward the server's ports](https://en.wikipedia.org/wiki/Port_forwarding) in your router (project defaults are 80 for redirection and 443 for secure connection).

#### TLS Certificate
As the communication is implemented through secure https requests you have to create a proper key and certificate for it. The easiest way is using openSSL with this command:

    openssl req -x509 -nodes -newkey rsa:1024 -keyout key.pem -out cert.pem -days 4096 -addext "subjectAltName = DNS:%your-domain%.com"

Make sure to add your chosen DDNS domain at the "**subjectAltName**" argument. After executing it: fill in the required fields.

### Core
The ESP is programmed in Arduno environment. For this you have to install the proper [core](https://github.com/esp8266/Arduino) (installation instructions are also included there).
### Library dependencies
These libraries can be installed from the Arduino library manager:
 * [EasyDDNS](https://github.com/ayushsharma82/EasyDDNS)
 * [ArduinoQueue](https://github.com/EinarArnason/ArduinoQueue)
 * [ArduinoJSON](https://github.com/bblanchon/ArduinoJson)
 * [NTP Client](https://github.com/arduino-libraries/NTPClient)

### Configuring
The server can be configured with the [Configuration.h](./Configuration.h) header file:
* SSID and PSWD: the SSID and the password of the local WiFi network to which the ESP connects
* DDNS properties: checkout [EasyDDNS's readme](https://github.com/ayushsharma82/EasyDDNS#readme) what they mean
* User-name and password: simple authentication credentials
* Up and down pins
* Total time of the shutter's operations of going up and down

You have to specify the key and certificate (created by OpenSSL) in the [Keys.h](./Keys.h) header file.



## Backlog
* implement configuring automatic zeroing at given time? (+clients too?)