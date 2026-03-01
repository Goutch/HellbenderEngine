#pragma once

#include "Core.h"
#include "string"

namespace HBE
{
#define error(s, ...) error_(s, __LINE__,__FILE__,__func__)
    typedef uint32_t LOG_FLAGS;

    enum LOG_FLAG
    {
        LOG_FLAG_PROFILE = 32,
        LOG_FLAG_DEBUG = 16,
        LOG_FLAG_MESSAGE = 8,
        LOG_FLAG_STATUS = 4,
        LOG_FLAG_WARNING = 2,
        LOG_FLAG_ERROR = 1,
        LOG_FLAG_NONE = 0,
        LOG_FLAG_ALL = 0xFFFFFFFF
    };

    class HB_API Log
    {

    private:
        static LOG_FLAGS log_flags;

    public:
        static void setFlags(uint32_t flags);

        static void debug(const std::string& s);
        static void profile(const std::string& s);
        static void message(const std::string& s);

        static void status(const std::string& s);

        static void warning(const std::string& s);

        static void error_(const std::string& s, unsigned int line, std::string file, std::string function);
        static uint32_t getFlags();
    };
}
