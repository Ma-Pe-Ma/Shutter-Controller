## Shutter Controller - Desktop Client

This is a simple app with a GUI to control the ESP server. The project is made with Qt.

### Build-instructions
With a properly set up Qt creator this project should compile without no problems.
 
Only these steps are needed to be done before building:
* in the pro file set your server's url and the location to the OpenSSL headers and library files
* copy your certificate file to the root directory of the sources
* include to source folder these headers: [cpp-http-lib](https://github.com/yhirose/cpp-httplib) and [json for modern c++](https://github.com/nlohmann/json)


### To-do
* restructure http request system
* make server url and certifate settable by user
* replace custom language system to qt's