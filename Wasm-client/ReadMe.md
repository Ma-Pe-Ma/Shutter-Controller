# Shutter Controller - WASM Client
Before reading this through make sure you have read the [project's ReadMe](/ReadMe.md).

This solution can be used both on desktop and on the web as a WASM module.
The latter version is built by the Emscripten sdk and it runs in a HTML canvas element.

## Library dependencies
The following dependencies are needed for both versions:
* [DearImGui](https://github.com/ocornut/imgui) - an immediate mode GUI library which 
* [Protocol buffers](https://github.com/protocolbuffers/protobuf) - library for creating, parsing and serializing binary messages
* [GLFW](https://github.com/glfw/glfw) - utility used to create OpenGL contexts, and windows

These libraries are only needed for the desktop version:
* [CPP-HTTP-LIB](https://github.com/yhirose/cpp-httplib) - a library which can be used to send HTTP requests (the WASM version uses Emscripten's fetch library for this)
* [OpenSSL](https://www.openssl.org/) - SSL/TLS handling library which needed for secure requests (the WASM version uses the browser's native solution)
* [STB](https://github.com/nothings/stb) - library which loads the icon for the desktop window (the WASM version is served with a favicon)

## Build instructions

The project uses CMake as the build system. First the protobuf project needs to be built and installed and then its installation location have to be specified in the [CmakeLists.txt](./CMakeLists.txt) (which explains the detailed steps for building and installing). The `.proto` compilation is executed automatically during building.

Apart from this, this module does not need further configuration if it's built as a WASM module. However the resources/built files have to be served with an [external server](../ESP8266_Server/ReadMe.md#Setting-up-the-external-resource-provider-server).

The serving HTML page has to be served with the ESP itself just like the [HTML+JS client](../HTML+JS-client/ReadMe.md#configuration)

For the desktop version only the [OpenSSL developer tools](https://wiki.openssl.org/index.php/Binaries) installation path is need to be set as it cannot be built with CMake.

## Using the desktop version
After building the following additional configuration is needed
* Add your server's (DDNS) URL to the `./resources/connection.txt` file
* Specify your username and password for request authentication in the aforementioned file (see the [server's Readme](/ESP8266_Server/ReadMe.md))
* Copy your certificate file to the `./resources/` directory
* Install OpenSSL user libraries (if the target system is different from the developing system)

## Backlog
* eliminate STB library