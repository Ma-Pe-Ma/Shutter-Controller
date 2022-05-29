#ifndef MESSAGES_H
#define MESSAGES_H

#define MESSAGE_NR 10
#include "json.hpp"
#include <QString>
#include <QMetaType>
#include "timing.h"

using json = nlohmann::json;

Q_DECLARE_METATYPE(std::string)

class Messages {
private:
    //static json messages[MESSAGE_NR];
    static QString messages[MESSAGE_NR];
    static QString dates[MESSAGE_NR];
    static QString startupDate;

public:
    Messages();
    static QString getProcessedMessage(int);
    static void parseGenericResponse(json);
    static QString processMessage(std::string, std::string, std::string);
    static QString getDate(int);
    static QString getStartupDate();
};

#endif // MESSAGES_H
