

#include "Clock.h"
namespace HBE {
    Clock::Clock() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    void Clock::stop() {
        //todo
    }

    void Clock::start() {
        //todo
    }

    void Clock::reset() {

        start_time = std::chrono::high_resolution_clock::now();
    }

    float Clock::ms() const {
        std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start_time;
        return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    }

    float Clock::ns() const {
        std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start_time;
        return static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
    }
}







