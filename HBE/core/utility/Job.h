#pragma once

#include <thread>
#include "Event.h"
#include <future>
#include "list"

template<typename ReturnType,typename Function,typename... Args>
class Job {
    std::future<ReturnType> result;
    static std::list<Job *> running_jobs;
private:
    Job(Function &&fun, Args &&... args) {
        this->result = std::async(std::launch::async, fun, args...);
    }

public:
    Event<ReturnType> onFinish;

    template<typename R,typename F,typename... A>
    static Job *run(F &&fun, A &&... args) {
        running_jobs.push_back(new Job<R,F, A...>(fun, args...));
    }

    static void doCallbacks() {
        auto it = running_jobs.begin();
        while (it != running_jobs.end()) {
            auto current = it++;
            if ((*current)->isFinish())
                running_jobs.erase(current);
            (*current)->onFinish.invoke();
        }
    }

    bool isFinish() {
        return result.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
    }

    ReturnType getReturnValue() {
        return result.get();
    }
};
