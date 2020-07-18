

#include "Timer.h"
Timer::Timer() {
    start=std::chrono::high_resolution_clock::now();
}
void Timer::reset() {

    start= std::chrono::high_resolution_clock::now();
}

float Timer::ms() const {
    std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
    return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}
double Timer::ns() const {
    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;
    return static_cast<double >(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
}








