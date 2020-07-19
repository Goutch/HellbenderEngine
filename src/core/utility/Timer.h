
#pragma once
#include <chrono>
#include <unordered_map>
class Timer {
    std::chrono::high_resolution_clock ::time_point start;

public:
    Timer();
    void reset();
    float ms() const;

    float ns() const;
};


