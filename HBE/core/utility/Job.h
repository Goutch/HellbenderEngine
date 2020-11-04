#pragma once

#include <thread>
#include "Function.h"
#include <future>
#include "list"


template<typename Return,typename... Args>
class Job {
    std::future<Return> result;
    std::function<void(Return)> finish_callback;

public:
    void run(Return(*static_function)(Args...),Args... args) {
        this->result = std::async(std::launch::async, static_function, args...);
    }

    template<typename Object>
    void run(Object *instance, Return(Object::* member_function)(Args...),Args... args) {
        this->result = std::async(member_function,instance, args...);
    }

    void setCallback(void(*static_callback)(Return))
    {
        finish_callback = std::bind(static_callback);
    }

    template<typename Object>
    void setCallback(Object *instance, void(Object::* member_function)(Return))
    {
        finish_callback = Attach(member_function,instance);
    }

    bool isFinish() {
        return result.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
    }

    void onFinish() {
        finish_callback(result.get());
    }

};
