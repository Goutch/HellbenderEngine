#pragma once
#include "Log.h"
#include "Clock.h"
#include "stack"
namespace HBE{
	class Profiler {
		struct Profile{
			std::string message;
			Clock clock;
		};
		static std::stack<Profile> stack;
	public:
		static void end();
		static void begin(std::string message);
	};
}


