

#include "Clock.h"

namespace HBE {
	Clock::Clock() {
		start_time = std::chrono::high_resolution_clock::now();
	}

	void Clock::stop() {
		if (stopped) return;
		total = total + (std::chrono::high_resolution_clock::now() - start_time);
		stopped = true;
	}

	void Clock::start() {
		if (!stopped) return;
		stopped = false;
		start_time = std::chrono::high_resolution_clock::now();
		//todo
	}

	void Clock::reset() {
		total = std::chrono::duration<double>::zero();
		stopped = false;
		start_time = std::chrono::high_resolution_clock::now();
	}

	double Clock::ms() const {
		if (stopped) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(total).count();
		} else {
			std::chrono::duration<double> duration = (std::chrono::high_resolution_clock::now() - start_time) + total;
			return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		}
	}

	double Clock::ns() const {
		if (stopped) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(total).count();
		} else {
			std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::nanoseconds>((std::chrono::high_resolution_clock::now() - start_time)) + total;
			return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
		}

	}
}







