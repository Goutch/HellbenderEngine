#include "Profiler.h"

namespace HBE {
	std::stack<Profile> Profiler::profile_stack;
	std::unordered_map<std::string, ProfileGraphNode *> Profiler::nodes;

	void Profiler::begin(std::string message) {
		ProfileGraphNode *current_node = nullptr;
		if (profile_stack.empty()) {

			auto it = nodes.find(message);
			if (it != nodes.end()) {
				current_node = it->second;
			} else {
				current_node = new ProfileGraphNode();
				current_node->message = message;
				nodes.emplace(message, current_node);
			}
		} else {
			current_node = profile_stack.top().node;
			auto it = current_node->sub_nodes.find(message);
			if (it != current_node->sub_nodes.end()) {
				current_node = it->second;
			} else {
				current_node = new ProfileGraphNode();
				current_node->message = message;
				profile_stack.top().node->sub_nodes.emplace(message, current_node);
			}
		}

		profile_stack.emplace(Profile{current_node, Timer()});
		profile_stack.top().clock.reset();
		std::string indent = "";
		for (int i = 0; i < profile_stack.size(); ++i) {
			indent += '-';
		}
		Log::profile(indent + message);
	}

	void Profiler::end(std::string message) {
		if (message != profile_stack.top().node->message) {
			Log::error("Profiler::end() message does not match begin() message, begin: " + profile_stack.top().node->message + " end: " + message);
		}
		double ms = profile_stack.top().clock.ns() * NANOSECONDS_TO_MILISECONDS;

		ProfileGraphNode *current_node = profile_stack.top().node;
		current_node->count++;

		if (profile_stack.top().node->max < ms) {
			profile_stack.top().node->max = ms;
			profile_stack.top().node->max_i = profile_stack.top().node->count;
		} else if (profile_stack.top().node->min > ms) {
			profile_stack.top().node->min = ms;
			profile_stack.top().node->min_i = profile_stack.top().node->count;
		}

		current_node->min = current_node->count > 1 ? std::min(profile_stack.top().node->min, ms) : ms;
		current_node->max = current_node->count > 1 ? std::max(profile_stack.top().node->max, ms) : ms;


		double total = current_node->time * (current_node->count - 1);
		current_node->time = (total + ms) / current_node->count;
		std::string indent = "";
		for (int i = 0; i < profile_stack.size(); ++i) {
			indent += '-';
		}
		Log::profile(indent + profile_stack.top().node->message + " end in " + std::to_string(ms));
		profile_stack.pop();
	}

	void printNodeAverange(ProfileGraphNode *node, int indent) {
		std::string indent_str_tabs = "";
		for (int i = 0; i < indent; ++i) {
			indent_str_tabs += "|    ";
		}
		Log::message(indent_str_tabs + "[" + node->message + "]" + std::to_string(node->count) + "\n" +
		             indent_str_tabs + "|avg: " + std::to_string(node->time) + "ms|" + "\n" +
		             indent_str_tabs + "|min: " + std::to_string(node->min) + "ms|#" + std::to_string(node->min_i) + "\n" +
		             indent_str_tabs + "|max: " + std::to_string(node->max) + "ms|#" + std::to_string(node->max_i));
		for (auto n: node->sub_nodes) {
			printNodeAverange(n.second, indent + 1);
		}
		delete node;
	}

	void Profiler::printAverange() {
		int indent = 0;
		for (auto n: nodes) {
			printNodeAverange(n.second, indent);
		}
	}
}

