#pragma once

#include "unordered_set"
#include "functional"
#include "Function.h"
#include "unordered_map"
#include "Core.h"
namespace HBE {
    template<typename... Args>

    class Event {
        std::unordered_set<void (*)(Args...)> static_callbacks;
        std::unordered_map<void *, std::function<void(Args...)>> instances_callbacks;
    public:
        void subscribe(void(*static_callback)(Args...)) {
            static_callbacks.insert(static_callback);
        }

        void unsubscribe(void(*static_callback)(Args...)) {
            static_callbacks.erase(static_callback);
        }

        template<typename Object>
        void subscribe(Object *instance, void(Object::* fun)(Args...)) {
            instances_callbacks.emplace(instance, Attach(fun, instance));
        }

        void unsubscribe(void *instance) {
            instances_callbacks.erase((void *) instance);
        }

        void invoke(Args... args) const {
            for (void (*c)(Args...) :static_callbacks) {
                c(args...);
            }
            for (const std::pair<void *, std::function<void(Args...)>> pair:instances_callbacks) {
                pair.second(args...);
            }
        };
    };

}