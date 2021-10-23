#include "messages.h"
#include <QDebug>
#include <languages/languages.h>

QString Messages::messages[MESSAGE_NR];
QString Messages::dates[MESSAGE_NR];
QString Messages::startupDate;

Messages::Messages() {

}

QString Messages::getProcessedMessage(int messageID) {
    return messages[messageID];
}

QString Messages::getDate(int messageID) {
    return dates[messageID];
}

void Messages::parseGenericResponse(json genericResponse) {
   // qInfo()<<"Generic response: "<<genericResponse.dump().c_str();

    int restartTime = genericResponse["T"].get<int>();
    Timing::setRestartTime(restartTime);

    if (restartTime > 0) {
        //qInfo()<<"RESP: OBJ: "<<genericResponse.dump().c_str();
        return;
    }

    int value = genericResponse["V"].get<int>();
    Timing::setCurrentValue(value);

    json messagesObject = genericResponse["M"].get<json>();

    std::string date = Languages::getFormattedStringByID(Languages::StringResource::startTime, messagesObject["S"].get<std::string>().c_str());
    startupDate = date.c_str();

    json genericMessages = messagesObject["M"].get<json>();

    //qInfo()<<"MES OBJ: "<<messagesObject.dump().c_str();
    //qInfo()<<"GEN MES: "<<genericMessages.dump().c_str();

    for (const auto& item : genericMessages.items()) {
        int ID = std::stoi(item.key());

        try {
            std::string type  = item.value()["T"].get<std::string>();
            std::string res   = item.value()["R"].get<std::string>();
            std::string add   = item.value()["A"].get<std::string>();
            Messages::messages[ID] = processMessage(type, res, add);
            if (Messages::messages[ID].length() > 1) {
                Messages::dates[ID] = item.value()["D"].get<std::string>().c_str();
            }
            else {
                Messages::dates[ID] = "-";
            }
        }
        catch(json::exception& ex) {
            //qInfo()<<"JsonError: "<<ex.byte;
            Messages::messages[ID] = "";
            Messages::dates[ID] = "-";
        }
    }
}

QString Messages::processMessage(std::string type, std::string res, std::string add) {    
    if (type == "E") {
        return "-";
    }

    if (type == "I") {        
        return Languages::getFormattedStringByID(Languages::StringResource::startEvent).c_str();
    }

    if (type == "J") {
        return Languages::getFormattedStringByID(Languages::StringResource::jsonError, res.c_str(), add.c_str()).c_str();
    }

    if (type == "Z") {
        if (res == "F") {
            return Languages::getFormattedStringByID(Languages::StringResource::nullFail).c_str();
        }
        if (res == "O") {
            if (add == "U") {
                return Languages::getFormattedStringByID(Languages::StringResource::nullUp).c_str();
            }
            else if (add == "D") {
                return Languages::getFormattedStringByID(Languages::StringResource::nullDown).c_str();
            }
        }
    }

    if (type == "T") {
        return Languages::getFormattedStringByID(Languages::StringResource::timingEvent, res.c_str(), add.c_str()).c_str();
    }

    if (type == "S") {
        if (res == "M") {
            return Languages::getFormattedStringByID(Languages::StringResource::manualSet, add.c_str()).c_str();
        }

        if (res == "Z") {
            return Languages::getFormattedStringByID(Languages::StringResource::positionFound, add.c_str()).c_str();
        }

        if (res == "T") {
            return Languages::getFormattedStringByID(Languages::StringResource::timingsSet).c_str();
        }
    }

    return Languages::getFormattedStringByID(Languages::StringResource::unknown, type.c_str(), res.c_str(), add.c_str()).c_str();
}

QString Messages::getStartupDate() {
    return startupDate;
}
