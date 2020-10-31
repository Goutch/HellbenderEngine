#pragma once

#include "unordered_set"

template <typename... Args> class Event {
    std::unordered_set<void(*)(Args...)> callbacks;

public:
    void operator += (void(* callback)(Args...))
    {
        callbacks.emplace(callback);
    }

    void operator -= (void(* callback)(Args...))
    {
        callbacks.erase(callback);
    }
    void invoke(Args... params) const{
        for (void(* c)(Args...) :callbacks) {
            c(params...);
        }
    };
};

