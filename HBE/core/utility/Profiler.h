#pragma once

#include "Core.h"
#include "Log.h"
#include "Clock.h"
#include "stack"
#include "unordered_map"

namespace HBE {
	struct ProfileGraphNode{
		std::string message;
		uint32_t count=0;
		double min=0;
		double max=0;
		double time=0;
		std::unordered_map<std::string,ProfileGraphNode*> sub_nodes;
	};
	struct Profile {
		ProfileGraphNode* node;
		Clock clock;
	};
	class HB_API Profiler {
		static std::stack<Profile> profile_stack;
		static std::unordered_map<std::string, ProfileGraphNode*> nodes;
	public:
		static void end(std::string message);
		static void begin(std::string message);
		static void printAverange();
	};
}