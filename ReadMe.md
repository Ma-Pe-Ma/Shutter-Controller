# Shutter Controller

This is a very simple solution for controlling motorized rolling shutters remotely.

## Features

### Working principle
The shutter is controlled by an [ESP8226](https://www.espressif.com/en/products/socs/esp8266), only two pins are used: one for signalling up, and one for signalling down. For both signals a seperate relay is used to turn on the power circuit to move the shutter to the proper direction (up or down).

There is no feedback for the current position of the shutter so it's only possible to operate it for a given amount of time. The speed of the rolling is assumed constant so a setting process's duration can be calculated by the current position, the next position and by the speed.

Because of this the ESP needs to be updated regularly with the correct current location. (The easiest way to set this at the two extremities full-up and full-down but also there is an automatic method which first finds one of the extremeties and then sets itself to the current position).

### Timings
An additional handy function are timings you can schedule settings at specific times on the given days of the week.

### Communication, clients
The ESP acts as an https server to which you can POST settings and GET the current state.

At this state there are two clients: an [Android](./Android%20Client) and a [Qt Windows desktop client](./Desktop%20Client).

I do not provide built versions for this project as there are some properties which are user-specific and because of laziness they are hard-coded into the applications. It's planned to fix this limitations in the future. 

Android Client   |  Windows Client
:-------------------------:|:-------------------------:
<img src="./Images/Android%20Client/Main.jpg" alt="drawing" width="300"/>  |  <img src="./Images/Desktop%20Client/Main.png" alt="drawing"/>

## Setting Up 
### Software
#### DDNS
Fix IP addresses are generally not available for end-users so this problem is usually bypassed by using a [DDNS service](https://en.wikipedia.org/wiki/Dynamic_DNS). Firstly you have to make sure your connection is not behind [NAT](https://en.wikipedia.org/wiki/Network_address_translation) otherwise you have to ask your internet service provider to solve this (usually this is available to users).

Next you have to register for a DDNS service provider (like No-IP, DynDNS, DuckDNS eg). You will have to choose a domain name and you will get a token which is needed for updating your address. For setting it up check out the [server's ReadMe](./ESP8266_Server/ReadMe.md). Don't forget to [forward the server's ports](https://en.wikipedia.org/wiki/Port_forwarding) (project defaults are 80 for redirection and 443 for secure connection).

#### TLS Certificate
As the communication is implemented through secure https requests you have to create a proper key and certificate for it. The easiest way is using openSSL with this command:

    openssl req -x509 -nodes -newkey rsa:1024 -keyout key.pem -out cert.pem -days 4096 -addext "subjectAltName = DNS:%your-domain%.com"

Make sure to add your chosen DDNS domain at the "subjectaltname" argument. After executing it fill in the required fields.

#### Server + clients
You can find the various solutions' configuring manual at their own ReadMe:
* [Server + browser client](./ESP8266_Server/ReadMe.md)
* [Android](./Android%20Client/ReadMe.md)
* [Desktop](./Desktop%20Client/ReadMe.md)

### Hardware

As mentioned before the engine is switched by a relay which is signalled by the ESP. At the top and the bottom state there have to be two limit switches which turn off the power circuits in case of accidental bad signalling by the ESP. This prevents the damaging and burning down of the engine.

The ESP I used is a simple 8-pin variant (which technically has two GPIO pins: 0 and 2 but the TX and RX pins can be reprogrammed to act as GPIO). The problem with the ESP8266 is that at startup both the 0 and 2 pins are have to be pulled up to start normally or in the other case it will enter [sketch-flashing mode](https://www.esp8266.com/wiki/doku.php?id=esp8266_gpio_pin_allocations). When a relay is attached to one of the pins it gets pulled down so it will not start properly so a controlling circuit is needed to be implemented which solves pulling up the pins at startup.

There are two easy solutions for this problem. For both of them I designed a [simple PCB](./Controlling%20Circuit/) with [EasyEda](https://easyeda.com/).

   --   |   Solution 1   |  Solution 2
:-------------------------:|:-------------------------:|:-------------------------:
Description | The outputs are controlled with a PNP transistor. In this case the ESP signal logic is needed to be inverted programatically to get the supposed signal at the output. | The outputs are simply pulled up with a pull-up transistor. The ESP signal logic is inverted here too as well as the output signals which means the relay has to be used invertedly.
Circuit | <img src="./Images/Hardware/Circuit-1.png" alt="drawing"/>  |  <img src="./Images/Hardware/Circuit-2.png" alt="drawing"/>
PCB | <img src="./Images/Hardware/PCB-1.png" alt="drawing"/>  |  <img src="./Images/Hardware/PCB-2.png" alt="drawing"/>

Resistor values:
* R0=1kΩ (used for pulling down GPIO0 to flash the ESP)
* R1=R4=1kΩ (used to prevent shorting when controlling the pins)
* R2=R5=10kΩ (used for pulling up pins and transistors)
* R3=R6=1kΩ (used with LEDs)

At Solution 1 the diodes serve as [flyback diodes](https://en.wikipedia.org/wiki/Flyback_diode) for the relays while the LEDs indicate the working state of the  connected relay. 

For more safety it is **strongly** recommended to configure the relays in a way that they stop each other if one them is in switched state (the easiest way is adding an other pair of relays).

## Developer Notes
I tried to keep this project as simple as it can be but after adding more and more simple functions and because of the limited memory of the ESP8266 the code became a bit like a spaghetti-code.

If I would start the project now I would develop it for a different platform. It's not planned to restructure this project as it does its job.

## Backlog
* Make hard-coded properties (certificate, passwords, url) configurable