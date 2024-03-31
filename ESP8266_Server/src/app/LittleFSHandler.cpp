#include "LittleFSHandler.h"
#include "TimeCalibration.h"

namespace LittleFSHandler {

    void initialize(bool format) {
        LittleFS.begin();
        LittleFS.setTimeCallback(TimeCalibration::customTimeSetter); 

        if (format) {
            LittleFS.format();
        }

        String failedStartupCount;
        readFile("fail.txt", failedStartupCount);
        int retryCount = failedStartupCount.toInt() < 1 ? 1 : failedStartupCount.toInt() + 1;

        if (retryCount > 2) {
            LittleFS.format();
        }
        else {
            writeFile("fail.txt", String(retryCount));
        }        
    }

    void writeFile(const char* path, const String& message) {
        writeFile(path, message.c_str());
    }

    void writeFile(const char* path, uint8_t* message, size_t size) {
        File file = LittleFS.open(path, "w");
        if (!file) {
            Serial.println("\t\tFailed to open file for writing");
            return;
        }
        file.setTimeCallback(TimeCalibration::customTimeSetter);

        if (file.write(message, size)) {
            Serial.println("\t\tFile written");
        }
        else {
            Serial.println("\t\tWrite failed");
        }

        delay(2000);
        file.close();
    }

    void writeFile(const char * path, const char * message) {
        Serial.printf("\t\tWriting file: %s\n", path);

        File file = LittleFS.open(path, "w");
        if (!file) {
            Serial.println("\t\tFailed to open file for writing");
            return;
        }
        file.setTimeCallback(TimeCalibration::customTimeSetter);
        
        //Serial.print("Writing file: ");
        //Serial.println(message);

        if (file.print(message)) {
            Serial.println("\t\tFile written");
        } else {
            Serial.println("\t\tWrite failed");
        } 

        delay(2000); // Make sure the CREATE and LASTWRITE times are different
        file.close();
    }

    void readFile(const char * path, String& target) {
        target = "";

        Serial.printf("\t\tReading file: %s\n", path);

        File file = LittleFS.open(path, "r");
        
        if (!file) {
            Serial.println("\t\tFailed to open file for reading");
        }
        file.setTimeCallback(TimeCalibration::customTimeSetter);

        //Serial.print("Read from file: ");
        
        while (file.available()) {
            target += file.readString();
        }
       
        //Serial.println(target);
        file.close();
    }

    void deleteFile(const char * path) {
        Serial.printf("Deleting file: %s\n", path);
        if (LittleFS.remove(path)) {
            Serial.println("File deleted");
        } else {
            Serial.println("Delete failed");
        }
    }

    void listDir(const char * dirname) {
        Serial.printf("Listing directory: %s\n", dirname);

        Dir root = LittleFS.openDir(dirname);

        while (root.next()) {
            File file = root.openFile("r");
            Serial.print("  FILE: ");
            Serial.print(root.fileName());
            Serial.print("  SIZE: ");
            Serial.print(file.size());
            time_t cr = file.getCreationTime();
            time_t lw = file.getLastWrite();
            file.close();
            struct tm * tmstruct = localtime(&cr);
            Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
            tmstruct = localtime(&lw);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
        }
    }

    void format() {
        LittleFS.format();
    }
}