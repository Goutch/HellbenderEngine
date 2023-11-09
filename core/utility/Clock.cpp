

#include "Timer.h"
namespace HBE {
    Timer::Timer() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    void Timer::stop() {
        //todo
    }

    void Timer::start() {
        //todo
    }

    void Timer::reset() {

        start_time = std::chrono::high_resolution_clock::now();
    }

    float Timer::ms() const {
        std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start_time;
        return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    }

    float Timer::ns() const {
        std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start_time;
        return static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
    }
}







