#pragma once

#include "Core.h"
#include "string"
namespace HBE {
#define error(s, ...) error_(s, __LINE__,__FILE__,__func__)

	class HB_API Log {
	public:
		enum LOG_FLAGS{
			PROFILE = 32,
			DEBUG = 16,
			MESSAGE = 8,
			STATUS = 4,
			WARNING = 2,
			ERROR = 1,
			NONE = 0,
			ALL = 99999,
		};
	private:
		static uint32_t log_flags;
	public:
		static void setFlags(uint32_t flags);

		static void debug(const std::string &s);
		static void profile(const std::string &s);
		static void message(const std::string &s);

		static void status(const std::string &s);

		static void warning(const std::string &s);

		static void error_(const std::string &s, unsigned int line, std::string file, std::string function);
		static uint32_t getFlags();
	};
}
