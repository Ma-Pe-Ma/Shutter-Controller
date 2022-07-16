#include "messages.h"
#include <QDebug>
#include <QObject>


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

    int restartTime = genericResponse["R"].get<int>();
    Timing::setRestartTime(restartTime);

    if (restartTime > 0) {
        //qInfo()<<"RESP: OBJ: "<<genericResponse.dump().c_str();
        return;
    }

    int value = genericResponse["V"].get<int>();
    Timing::setCurrentValue(value);

    json messagesObject = genericResponse["M"].get<json>();

    if (messagesObject.empty()) {
        return;
    }

    startupDate = QObject::tr("startTime%1").arg(messagesObject["S"].get<std::string>().c_str());

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
        return QObject::tr("startEvent");
    }

    if (type == "J") {
        return QObject::tr("jsonError %1, %2").arg(res.c_str(), add.c_str());
    }

    if (type == "Z") {
        if (res == "F") {
            return QObject::tr("nullFail");
        }
        if (res == "O") {
            if (add == "U") {
                return QObject::tr("nullUp");
            }
            else if (add == "D") {
                return QObject::tr("nullDown");
            }
        }
    }

    if (type == "T") {
        return QObject::tr("timingEvent%1, %2").arg(res.c_str(), add.c_str());
    }

    if (type == "S") {
        if (res == "M") {
            return QObject::tr("manualSet %1").arg(add.c_str());
        }

        if (res == "Z") {
            return QObject::tr("positionFound %1").arg(add.c_str());
        }

        if (res == "T") {
            return QObject::tr("timingSet"); 
        }
    }

    return QObject::tr("unknown %1, %2, %3").arg(type.c_str(), res.c_str(), add.c_str());
}

QString Messages::getStartupDate() {
    return startupDate;
}
