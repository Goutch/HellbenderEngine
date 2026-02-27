//
// Created by User on 2020-01-17.
//

#include <stdexcept>
#include "Log.h"
#include "iostream"
#include "HBE/core/Application.h"

namespace HBE {
	uint32_t Log::log_flags = ERROR | STATUS | WARNING | MESSAGE | DEBUG ;


	void Log::setFlags(uint32_t flags) {
		log_flags = flags;
	}

	uint32_t Log::getFlags() {
		return log_flags;
	}

	void Log::debug(const std::string &s) {
		if (Log::log_flags & DEBUG) {
			std::cout << "[DEBUG|" + std::to_string(Application::getTime()) + "]" << s << std::endl;
		}
	}

	void Log::message(const std::string &s) {
		if (Log::log_flags & MESSAGE) {
			std::cout << s << std::endl;
		}
	}

	void Log::status(const std::string &s) {
		if (Log::log_flags & STATUS) {
			std::cout << "[STATUS|" + std::to_string(Application::getTime()) + "]" << s << std::endl;
		}
	}

	void Log::error_(const std::string &s, unsigned int line, std::string file, std::string function) {
		if (Log::log_flags & ERROR) {
			std::cerr << "[ERROR|" + std::to_string(Application::getTime()) + "]" + s + "\nfile :" + file + " at " + std::to_string(line)
					  << std::endl;
			throw std::runtime_error("error");
		}
	}

	void Log::warning(const std::string &s) {
		if (Log::log_flags & WARNING) {
			std::clog << "[WARNING]" << s << std::endl;
		}
	}

	void Log::profile(const std::string &s) {
		if (Log::log_flags & PROFILE) {
			std::cout << "[PROFILE]" << s << std::endl;
		}
	}


}




