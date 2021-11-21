# Shutter Controller - Desktop Client
This is a simple app with a GUI to control the ESP server. The project is made with Qt.
Before reading this through make sure you have read the [project's ReadMe](/ReadMe.md).

## Library dependencies
* [cpp-http-lib](https://github.com/yhirose/cpp-httplib) (header-only)
* [json for modern c++](https://github.com/nlohmann/json) (header-only)
* [openSSL](https://www.openssl.org/)

## Build instructions
With a properly set up Qt creator this project should compile without no problems. Before building you have to install openSSL to your computer. You can find [binaries here](https://wiki.openssl.org/index.php/Binaries). Make sure you choose the correct version for the compiler you use in QT!

### Configuration 
Before building  the following properties neeed to be configured in the project's .pro file:
* Your server's (DDNS) URL
* Username and password for request authentication (see the [server's Readme](/ESP8266_Server/ReadMe.md))
* Location to the OpenSSL headers and library files
* Copy to source folder the "cpp-http-lib" and "json for modern c++" headers

After building:
* Copy your certificate file to the root directory of the built application
* Deploy the QT application (on Windows use the windeployqt tool) to run it in other non-developer environments
* (Attach openSSL DLL/install openSSL on other non-developer environments)

## Back Log
* Make server url and certifate settable by user
* Replace custom language system to Qt's
* Implementing automatically adjusting GUI