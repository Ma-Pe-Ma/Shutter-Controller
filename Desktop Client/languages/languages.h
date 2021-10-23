#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <map>
#include <string>

#include <stdexcept>
#include <memory>

namespace Languages {

    enum class StringResource : int {
        appName,
        syncing,
        startTime,
        up,
        down,
        setPos,
        timings,
        nulling,
        messages,
        startEvent,
        jsonError,
        nullFail,
        nullUp,
        nullDown,
        timingEvent,
        manualSet,
        positionFound,
        timingsSet,
        unknown,
        w0,
        w1,
        w2,
        w3,
        w4,
        w5,
        w6,
        serverUnavailable,
        serverAvailable,
        eventLabel,
        dateLabel,
        setShutter,
        setButton,
        automatic,
        zeroQuestion,
    };

    extern std::map<std::string, StringResource> variableMap;

    extern std::map<StringResource, std::string> languageMap;

    void initialize();

    template<typename ... Args>
    std::string getFormattedStringByID(Languages::StringResource stringResource, Args ... args) {
        std::string formattable = languageMap[stringResource];

        int size_s = std::snprintf( nullptr, 0, formattable.c_str(), args ... ) + 1; // Extra space for '\0'
        if(size_s <= 0 ) {
            throw std::runtime_error("Error during formatting.");
        }

        auto size = static_cast<size_t>(size_s);
        auto buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, formattable.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }
}

#endif
