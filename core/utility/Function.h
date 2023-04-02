#pragma once

#include "functional"
namespace HBE {
    template<typename Return, typename Object, typename... Args>
    std::function<Return(Args...)> Attach(Return (Object::*function)(Args...), Object *instance) {
        return [instance, function](Args... args) -> Return { return (instance->*function)(args...); };
    };
}