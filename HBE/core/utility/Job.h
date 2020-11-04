#pragma once

#include <thread>
#include "Event.h"
#include <future>
#include "list"


template<typename... Args>
class Job {
    std::future<void> result;
    std::function<void()> finish_callback;

public:


    void run(void(*static_function)(Args...),Args... args) {
        this->result = std::async(std::launch::async, static_function, args...);
    }
    template<typename T>
    void run(T *instance, void(T::* member_function)(Args...),Args... args) {
        this->result = std::async(member_function,instance, args...);
    }

    void setCallback(void(*static_callback)())
    {
        finish_callback = std::bind(static_callback);
    }
    template<typename T>
    void setCallback(T *instance, void(T::* member_function)())
    {
        finish_callback = std::bind(member_function, instance);
    }

    bool isFinish() {
        return result.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
    }

    void onFinish() {
        finish_callback();
    }

};
