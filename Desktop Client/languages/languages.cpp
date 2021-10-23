#include "languages/languages.h"

#include <QDebug>
#include <QApplication>
#include <QDir>
#include <fstream>
#include <QDirIterator>

namespace Languages {

    std::map<Languages::StringResource, std::string> languageMap;
    std::map<std::string, Languages::StringResource> variableMap = {
        {"appName", Languages::StringResource::appName},
        {"syncing", Languages::StringResource::syncing},
        {"startTime", Languages::StringResource::startTime},
        {"up", Languages::StringResource::up},
        {"down", Languages::StringResource::down},
        {"setPos", Languages::StringResource::setPos},
        {"timings", Languages::StringResource::timings},
        {"nulling", Languages::StringResource::nulling},
        {"messages", Languages::StringResource::messages},
        {"startEvent", Languages::StringResource::startEvent},
        {"jsonError", Languages::StringResource::jsonError},
        {"nullFail", Languages::StringResource::nullFail},
        {"nullUp", Languages::StringResource::nullUp},
        {"nullDown", Languages::StringResource::nullDown},
        {"timingEvent", Languages::StringResource::timingEvent},
        {"manaulSet", Languages::StringResource::manualSet},
        {"positionFound", Languages::StringResource::positionFound},
        {"timingsSet", Languages::StringResource::timingsSet},
        {"unknown", Languages::StringResource::unknown},
        {"w0", Languages::StringResource::w0},
        {"w1", Languages::StringResource::w1},
        {"w2", Languages::StringResource::w2},
        {"w3", Languages::StringResource::w3},
        {"w4", Languages::StringResource::w4},
        {"w5", Languages::StringResource::w5},
        {"w6", Languages::StringResource::w6},
        {"serverUnavailable", Languages::StringResource::serverUnavailable},
        {"serverAvailable", Languages::StringResource::serverAvailable},
        {"eventLabel", Languages::StringResource::eventLabel},
        {"dateLabel", Languages::StringResource::dateLabel},
        {"setShutter", Languages::StringResource::setShutter},
        {"setButton", Languages::StringResource::setButton},
        {"automatic", Languages::StringResource::automatic},
        {"zeroQuestion", Languages::StringResource::zeroQuestion},
    };

    void initialize() {
        QString localeID = QLocale().bcp47Name();
        QString langPath = ":/app/languages/";

        QString fileName = "en";

        QDirIterator it(":/app/languages", QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            QString resourceName = it.fileName();
            resourceName.truncate(resourceName.lastIndexOf('.'));

            if (QString::compare(localeID, resourceName, Qt::CaseInsensitive) == 0) {
                fileName = resourceName;
                break;
            }

        }

        langPath = langPath.append(fileName);
        langPath.append(".txt");

        QFile file(langPath);

        if (file.open(QFile::ReadOnly)) {
            char buf[1024];

            while (qint64 lineLength = file.readLine(buf, sizeof(buf))> -1) {
                std::string line = std::string(buf);
                line = line.substr(0, line.size() - 2);

                std::string key = line.substr(0, line.find(' '));
                std::string value = line.substr(line.find(' ') + 1);

                Languages::StringResource stringResource = variableMap[key];
                languageMap.insert(std::pair<StringResource, std::string>(stringResource, value));
            }
        }
    }
}

