#pragma once

#include <thread>
#include "Function.h"
#include <future>
#include "Log.h"

class IJob {
public:
    virtual bool isFinish() const = 0;

    virtual void onFinish() const = 0;
};

template<typename Return, typename... Args>
class Job : public IJob {
    friend class JobManager;
    std::future<Return> result;
    std::function<void(Return)> finish_callback;
    bool has_callback = false;
    bool running = false;
    Job(){};
public:
    void run(Return(*static_function)(Args...), Args... args) {
        if (!running) {
            running = true;
            this->result = std::async(std::launch::async, static_function, args...);
        } else {
            Log::warning("Trying to start a job already running!");
        }
    }

    template<typename Object>
    void run(Object *instance, Return(Object::* member_function)(Args...), Args... args) {
        if (!running) {
            running = true;
            this->result = std::async(member_function, instance, args...);
        } else {
            Log::warning("Trying to start a job already running!");
        }
    }

    void setCallback(void(*static_callback)(Return)) {
        has_callback = true;
        finish_callback = std::bind(static_callback);
    }

    template<typename Object>
    void setCallback(Object *instance, void(Object::* member_function)(Return)) {
        has_callback = true;
        finish_callback = Attach(member_function, instance);
    }

    bool isFinish() {
        return result.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
    }

    void onFinish() {
        running = false;
        if (has_callback)
            finish_callback(result.get());
        has_callback = false;
    }

};


