#pragma once

#include <thread>

#include <future>

class Job {
public:
    std::future<void> result;

    template<typename Fun, typename... Args>
    Job(Fun &&fun, Args &&... args) {
        this->result = std::async(std::launch::async, fun, args...);
    }

    bool isFinish();
};