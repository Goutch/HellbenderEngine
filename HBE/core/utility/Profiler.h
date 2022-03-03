#pragma once

#include "Core.h"
#include "Log.h"
#include "Clock.h"
#include "stack"
#include "unordered_map"

namespace HBE {
	class HB_API Profiler {
		struct Profile {
			std::string message;
			Clock clock;
		};
		static std::stack<Profile> stack;
		static std::unordered_map<std::string, std::pair<uint32_t, double>> averange;
	public:
		static void end();
		static void begin(std::string message);
		static void printAverange();
	};
}


