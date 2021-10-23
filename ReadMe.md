# Shutter Controller


This is a very simple solution for controlling motored rolling shutters remotely.

The shutter is controlled by an ESP 8226, only two pins are used: one for signalling up, and one for signalling down. For both signals a seperate relay is used to turn on the power circuit to move the shutter to the proper direction. At the top and the bottom state there have to be two limit switches which turn off the power circuits in case of accidental bad signalling by the ESP. This prevents the damaging and burning down of the engine.

There is no feedback for the current position of the shutter so the operation time of the setting to the required position is measured and calulated by the speed of the moving shutter and the displacement (assuming the speed is constant). Because of this the ESP needs to be updated regularly with some reference location. (You can choose up and down and there is an automatic method which first finds one of the extremeties and then sets itself to the current position).

An additional handy function are timings you can schedule settings at specific times on the given days of the week.

The ESP acts as an http server to which you can POST settings and GET the current state.

At this state there are two clients: an [Android](./), and a [qt windows desktop](./) windows desktop client.


# Setting Up 
* DDNS
* Shutter speed
* SSL Certificate
* Modifying time library
* (Passwords)

# To do:
General:

* implementing simple browser client (https://github.com/esp8266/ESPWebServer/blob/master/examples/SimpleAuthentification/SimpleAuthentification.ino)
* implement passwording
* languages

ESP:
* implement solution for ESP time overflow (or notification about restart?)
* implement configuring automatic zeroing at given time (+clients too)

Android client:
* cleanup GUI + create more evident feedback events + disabling gui when request is active
* fix multiple request problem
* (probably certificate choosing from internal storage?)

Desktop client:
* restructure request sytem
* implementing 5automatically adjusting gui