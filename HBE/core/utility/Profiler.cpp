//
// Created by user on 11/10/2021.
//

#include "Profiler.h"

namespace HBE {
	std::stack<Profiler::Profile> Profiler::stack;
	std::unordered_map<std::string, std::pair<uint32_t, double>> Profiler::averange;

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
		auto it = averange.find(stack.top().message);
		if (it == averange.end()) {
			averange.emplace(stack.top().message, std::make_pair(1, stack.top().clock.ns() * NANOSECONDS_TO_MILISECONDS));
		} else {
			uint32_t count = averange[stack.top().message].first;
			double total = averange[stack.top().message].second * count;
			averange[stack.top().message].second = (total + (stack.top().clock.ns() * NANOSECONDS_TO_MILISECONDS)) / (count+1);
			averange[stack.top().message].first = count + 1;
		}
		stack.pop();
	}

	void Profiler::printAverange() {
		for (std::pair<std::string, std::pair<uint32_t, double>> message_key_and_count_value_pair:averange) {
			Log::message("[" + message_key_and_count_value_pair.first + "]" + std::to_string(message_key_and_count_value_pair.second.second) + "ms");
		}
	}
}

