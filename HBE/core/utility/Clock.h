
#pragma once
#include <chrono>

#define SECONDS_TO_NANOSECOND 1000000000

#define SECOND_TO_MILISECOND 1000
class Clock {
    std::chrono::high_resolution_clock ::time_point start;

public:
    Clock();
    void reset();
    void start();
    void stop();
    float ms() const;

    float ns() const;
};


