# Shutter Controller - Android Client
Before reading this through make sure you have read the [project’s ReadMe](/ReadMe.md).
A fully configured Android Studio project is provided. Only a few things need to be configured. For the dependencies see the module's build.gradle file.

## Configuring
In the build.gradle the following parameters are needed to be set
* Your server’s (DDNS) URL
* Username and password for request authentication (see the [server’s Readme](/ESP8266_Server/ReadMe.md))


Also you have to copy the generated certificate file (cert.pem) to "app/res/raw/cert.pem"

## Back Log
* cleanup GUI + create more evident feedback events + disabling gui when request is active
* (probably certificate choosing from internal storage?)