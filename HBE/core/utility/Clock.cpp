

#include "Clock.h"
Clock::Clock() {
    start=std::chrono::high_resolution_clock::now();
}
void Clock::reset() {

    start= std::chrono::high_resolution_clock::now();
}

float Clock::ms() const {
    std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
    return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}
float Clock::ns() const {
    std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
    return static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
}








