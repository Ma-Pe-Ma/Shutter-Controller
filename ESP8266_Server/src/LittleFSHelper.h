#ifndef LITTLEFSHELPER_H
#define LITTLEFSHELPER_H

#include <LittleFS.h>

namespace LittleFSHelper {
    void initialize(bool format = false);
    void writeFile(const char* path, const String& message);
    void writeFile(const char * path, const char * message);
    void readFile(const char * path, String& target);
    void deleteFile(const char * path);
    void listDir(const char * dirname);
}

#endif