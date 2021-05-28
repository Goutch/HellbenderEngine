#pragma once
#include "Core.h"
#include "string"
namespace HBE {
    #define error(s, ...) error_(s, __LINE__,__FILE__,__func__)

    class HB_API Log {
    public:
        enum LogLevel {
            DEBUG = 5,
            MESSAGE = 4,
            STATUS = 3,
            WARNING = 2,
            ERROR = 1,
            NOTHING = 0,
        };
    private:
        static LogLevel log_level;
    public:
        static void logLevel(LogLevel level);

        static void debug(const std::string &s);

        static void message(const std::string &s);

        static void status(const std::string &s);

        static void warning(const std::string &s);

        static void error_(const std::string &s, unsigned int line, std::string file, std::string function);
    };
}