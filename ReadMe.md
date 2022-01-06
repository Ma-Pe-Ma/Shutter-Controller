# Shutter Controller

This is a very simple solution for controlling motorized rolling shutters remotely.

The shutter is controlled by an ESP 8226, only two pins are used: one for signalling up, and one for signalling down. For both signals a seperate relay is used to turn on the power circuit to move the shutter to the proper direction (up or down). At the top and the bottom state there have to be two limit switches which turn off the power circuits in case of accidental bad signalling by the ESP. This prevents the damaging and burning down of the engine.

There is no feedback for the current position of the shutter so it's only possible to operate it for a given amount of time. The speed of the rolling is assumed constant so a setting process's duration can be calculated by the current position, the next position and by the speed.

Because of this the ESP needs to be updated regularly with the correct current location. (The easiest way to set this at the two extremities full-up and full-down but also there is an automatic method which first finds one of the extremeties and then sets itself to the current position).

An additional handy function are timings you can schedule settings at specific times on the given days of the week.

The ESP acts as an https server to which you can POST settings and GET the current state.

At this state there are two clients: an [Android](./Android%20Client) and a [Qt Windows desktop client](./Desktop%20Client).

I do not provide built versions for this project as there are some properties which are user-specific and because of laziness they are hard-coded into the applications. It's planned to fix this limitations in the future. 

Android Client   |  Windows Client
:-------------------------:|:-------------------------:
<img src="./Images/Android%20Client/Main.jpg" alt="drawing" width="300"/>  |  <img src="./Images/Desktop%20Client/Main.png" alt="drawing"/>

## Setting Up 
### DDNS
Fix IP addresses are generally not available for end-users so this problem is usually bypassed by using a [DDNS service](https://en.wikipedia.org/wiki/Dynamic_DNS). Firstly you have to make sure your connection is not behind [NAT](https://en.wikipedia.org/wiki/Network_address_translation) otherwise you have to ask your internet service provider to solve this (usally this is available to users).

Next you have to register for a DDNS service provider (like No-IP, DynDNS, DuckDNS eg). You will have to choose a domain name and you will get a token which is needed for updating your address. For setting it up check out the [server's ReadMe](./ESP8266_Server/ReadMe.md). Don't forget to [forward the server's ports](https://en.wikipedia.org/wiki/Port_forwarding) (project defaults are 80 for redirection and 443 for secure connection).

### TLS Certificate
As the communication is implemented through secure https requests you have to create a proper key and certificate for it. The easiest way is using openSSL with this command:

    openssl req -x509 -nodes -newkey rsa:1024 -keyout key.pem -out cert.pem -days 4096 -addext "subjectAltName = DNS:yourip.com"

Be sure to add your chosen DDNS domain at the subjectaltname argument. After executing it fill in the required fields.

### Server + clients
You can find the various solutions' configuring manual at their own ReadMe:
* [server](./ESP8266_Server/ReadMe.md)
* [android](./Android%20Client/ReadMe.md)
* [desktop](./Desktop%20Client/ReadMe.md)

## Backlog
* implementing simple browser client (cookies: https://github.com/esp8266/ESPWebServer/blob/master/examples/SimpleAuthentification/SimpleAuthentification.ino)
* make hard-coded properties (certificate, passwords, url) configurable