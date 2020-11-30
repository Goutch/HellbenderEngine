
#pragma once
#include <chrono>

#define SECONDS_TO_NANOSECOND 1000000000

#define SECOND_TO_MILISECOND 1000
#include "Core.h"
namespace HBE {
    class HB_API Clock {
        std::chrono::high_resolution_clock::time_point start_time;

    public:
        Clock();

        void reset();

        void start();

        void stop();

        float ms() const;

        float ns() const;
    };

}
