//
// Created by user on 11/10/2021.
//

#include "Profiler.h"

namespace HBE {
	std::stack<Profiler::Profile> Profiler::stack;

	void HBE::Profiler::begin(std::string message) {
		stack.emplace(Profile{message, Clock()});
		std::string messaage = "";
		for (size_t i = 0; i < stack.size() - 1; ++i) {
			messaage += "\t";
		}
		Log::profile(messaage + "[Profile:" + stack.top().message + "]");

	}

	void Profiler::end() {
		std::string messaage = "";
		for (size_t i = 0; i < stack.size() - 1; ++i) {
			messaage += "\t";
		}
		Log::profile(messaage + "[End:" + stack.top().message + "]" + std::to_string(stack.top().clock.ns() * NANOSECONDS_TO_MILISECONDS) + "ms");
		stack.pop();
	}
}

