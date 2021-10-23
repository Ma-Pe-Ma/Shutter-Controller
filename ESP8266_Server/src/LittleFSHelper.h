#ifndef LITTLEFSHELPER_H
#define LITTLEFSHELPER_H

#include <LittleFS.h>

namespace LittleFSHelper {
    void initialize();
    void writeFile(const char* path, String message);
    void writeFile(const char * path, const char * message);
    String readFile(const char * path);
    void deleteFile(const char * path);
    void listDir(const char * dirname);
}

#endif