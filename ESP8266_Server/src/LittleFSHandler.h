#ifndef LITTLEFSHANDLER_H
#define LITTLEFSHANDLER_H

#include <LittleFS.h>

namespace LittleFSHandler {
    void initialize(bool format = false);
    void writeFile(const char* path, const String& message);
    void writeFile(const char * path, const char * message);
    void writeFile(const char* path, uint8_t* message, size_t size);
    void readFile(const char * path, String& target);
    void deleteFile(const char * path);
    void listDir(const char * dirname);
    void format();
}

#endif