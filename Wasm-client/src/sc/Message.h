#ifndef MESSAGE_H
#define MESSAGE_H

#include <map>
#include <string>
#include <memory>

#include "Shutter.pb.h"

class Message {
    inline static std::shared_ptr<std::map<std::string, std::string>> translation;

    std::string date = "-";
    std::string processedMessage = "-";
    Shutter::Event event;
    int value;

public:
    inline static void setTranslation(std::shared_ptr<std::map<std::string, std::string>> translation) {
        Message::translation = translation;
    }

    Message() {}

    Message(Shutter::Event event, int value, std::string date) {
        this->event = event;
        this->value = value;
        this->date = date;

        std::string key = Shutter::Event_descriptor()->FindValueByNumber(event)->name();
        this->processedMessage = std::vformat((*Message::translation)[key], std::make_format_args(value));
    }

    friend class DemoRequest;
    friend class MainWindow;
};

#endif