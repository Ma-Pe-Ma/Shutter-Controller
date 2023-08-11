#ifndef MESSAGE_H
#define MESSAGE_H

#include <map>
#include <string>
#include <memory>

class Message {
    inline static std::shared_ptr<std::map<std::string, std::string>> translation;
    std::string date = "-";
    std::string processedMessage = "-";

public:
    inline static void setTranslation(std::shared_ptr<std::map<std::string, std::string>> translation) {
        Message::translation = translation;
    }

    Message() {}

	Message(std::string type, std::string res, std::string add, std::string date)
	{
        this->date = date;

        if (type == "E") {
            processedMessage = "-";
        }

        else if (type == "I") {
            processedMessage = (*Message::translation)["startEvent"];
        }

        else if (type == "J") {
            processedMessage = std::vformat((*Message::translation)["jsonError"], std::make_format_args(res, add));
        }

        else if (type == "Z") {
            if (res == "F") {
                processedMessage = (*Message::translation)["nullFail"];
            }
            if (res == "O") {
                if (add == "U") {
                    processedMessage = (*Message::translation)["nullUp"];
                }
                else if (add == "D") {
                    processedMessage = (*Message::translation)["nullDown"];
                }
            }
        }

        else if (type == "T") {
            processedMessage = std::vformat((*Message::translation)["timingEvent"], std::make_format_args(res, add));
        }

        else if (type == "S") {
            if (res == "M") {
                processedMessage = std::vformat((*Message::translation)["manualSet"], std::make_format_args(add));
            }

            if (res == "Z") {
                processedMessage = std::vformat((*Message::translation)["positionFound"], std::make_format_args(add));
            }

            if (res == "T") {
                processedMessage = (*Message::translation)["timingSet"];
            }
        }
        else {
            processedMessage = std::vformat((*Message::translation)["unknown"], std::make_format_args(type, res, add));
        }
	}

    friend class MainWindow;
};

#endif