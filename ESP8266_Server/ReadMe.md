# Shutter Controller - ESP8266 server
Before reading this through make sure you have read the [project's ReadMe](/ReadMe.md).

Important note: the ESP reboots after every 48 days automatically to prevent time overflowing.

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
 * [ArduinoQueue](https://github.com/EinarArnason/ArduinoQueue)
 * [NTP Client](https://github.com/arduino-libraries/NTPClient)

The [nanopb](https://github.com/nanopb/nanopb) library has to be downloaded manually, and the `pb_common.*`, `pb_decode.*`, `pb_encode.*` files have to be copied to the [src](./src/) directory.

### Configuring
The server can be configured with the [Configuration.h](./Configuration.h) header file:
* SSID and PSWD: the `SSID` and the `password` of the local WiFi network to which the ESP connects
* DuckDNS properties: the project used the DuckDNS for DDNS, the used `domain` and its `token` have to be specified
* `User-name` and `password`: simple authentication credentials
* Up and down pins
* Total time of the shutter's operations of going up and down
* the path where the resources of the HTML+JS/WASM clients are served

You have to specify the key and certificate (created by OpenSSL) in the [Keys.h](./Keys.h) header file.

### Setting up the external resource provider server

On the external server the HTML+JS and the WASM clients' resources have to be placed in the same folder.
The resources can be found at the release section.

The server has to serve the files with the following headers:

* `Access-Control-Allow-Origin: your-dns.com` - allow only our application's site to fetch the resources
* `Cross-Origin-Resource-Policy: cross-origin` - enable other sites to use these files as resources
* `Cache-Control: max-age=31536000` - cache the resources for at least a year as they will not change

### Protobuf configuration

The [Shutter.proto](./src/Shutter.proto) descriptor file has to be compiled with nanopb's protoc with the following commands.

    set proto_path=/ShutterController/ESP8266_Server/src/
    protoc --nanopb_out=%proto_path% --nanopb_opt=-I%proto_path% -I%proto_path% Shutter.proto
