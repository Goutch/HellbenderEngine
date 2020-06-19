//
// Created by User on 2020-01-17.
//

#include <stdexcept>
#include "Log.h"
#include "iostream"

Log::LogLevel Log::log_level = Log::DEBUG;

void Log::logLevel(Log::LogLevel level) {
    Log::log_level = level;
}

void Log::debug(const std::string &s) {
    if (Log::log_level >= DEBUG) {
        std::cout<< "[DEBUG]" << s << std::endl;
    }
}

void Log::message(const std::string &s) {
    if (Log::log_level >= MESSAGE) {
        std::cout<< "[MESSAGE]" << s << std::endl;
    }
}

void Log::status(const std::string &s) {
    if (Log::log_level >= STATUS) {
        std::cout<< "[STATUS]" << s << std::endl;
    }
}
void Log::error_(const std::string &s, unsigned int line, std::string file, std::string function) {
    if (Log::log_level >= ERROR) {
        std::cerr<<"[ERROR]"+s+"\nfile :"+file+" at "+std::to_string(line)<<std::endl;
        throw std::runtime_error("error") ;
    }
}

void Log::warning(const std::string &s) {
    if (Log::log_level >= WARNING) {
        std::clog << "[WARNING]" << s << std::endl;
    }
}






