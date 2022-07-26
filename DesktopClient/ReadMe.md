# Shutter Controller - Desktop Client
This is a simple app with a GUI to control the ESP server. The project is made with Qt.
Before reading this through make sure you have read the [project's ReadMe](/ReadMe.md).

## Library dependencies
* [cpp-http-lib](https://github.com/yhirose/cpp-httplib) (header-only)
* [json for modern c++](https://github.com/nlohmann/json) (header-only)
* [openSSL](https://www.openssl.org/)

## Build instructions
With a properly set up Qt Creator this project should compile without problems. Before building you have to install the OpenSSL developer tools to your computer. You can find [binaries here](https://wiki.openssl.org/index.php/Binaries). Make sure you choose the correct version for the compiler you use! (On Windows MSVC compiler worked for me only.)

### Configuration 
Before building  the following properties neeed to be configured in the project's .pro file:
* Your server's (DDNS) URL
* Username and password for request authentication (see the [server's Readme](/ESP8266_Server/ReadMe.md))
* Location to the OpenSSL headers and library files
* Copy the "cpp-http-lib" and "json for modern c++" headers to the root of this client's project

After building:
* Copy your certificate file to the root directory of the built application
* Deploy the QT application (on Windows use the windeployqt tool) to run it in other non-developer environments
* Install OpenSSL user libraries (if target system is different from the developing system)

## Backlog
* make hard-coded properties (certificate, passwords, url) configurable
* switch to automatically adjusting GUI