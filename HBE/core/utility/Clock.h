#pragma once

#include "Core.h"
#include <chrono>

#define SECONDS_TO_NANOSECONDS 1000000000.0
#define MILISECONDS_TO_NANOSECONDS 1000000.0
#define SECONDS_TO_MILISECONDS 1000.0
#define NANOSECONDS_TO_SECONDS 1.0/1000000000.0
#define NANOSECONDS_TO_MILISECONDS 1.0/1000000.0
#define MILISECONDS_TO_SECONDS 1.0/1000.0
namespace HBE {
	class HB_API Clock {
		std::chrono::high_resolution_clock::time_point start_time;
		std::chrono::duration<double> total;
		bool stopped = false;
	public:
		Clock();

		void reset();

		void start();

		void stop();

		double ms() const;

		double ns() const;
	};

}
