#pragma once

#include "unordered_set"
#include "functional"
template <typename... Args> class Event {
    std::unordered_set<void(*)(Args...)> static_callbacks;
    std::unordered_map<void*,std::function<void(Args...)>> instances_callbacks;
public:
    void subscribe(void(* static_callback)(Args...))
    {
        static_callbacks.insert(static_callback);
    }
    void unsubsribe(void(* static_callback)(Args...))
    {
        static_callbacks.erase(static_callback);
    }
    template<typename T>
    void subscribe(T* instance,void(T::* fun)(Args...) )
    {
        instances_callbacks.emplace((void*)instance,std::bind(fun, instance,std::placeholders::_1,std::placeholders::_2));
    }
    void unsubsribe(void* instance)
    {
        instances_callbacks.erase((void*)instance);
    }

    void invoke(Args... args) const{
        for (void(* c)(Args...) :static_callbacks) {
            c(args...);
        }
        for (const std::pair<void *, std::function<void(int, int)>> pair:instances_callbacks) {
            pair.second(args...);
        }
    };
};

