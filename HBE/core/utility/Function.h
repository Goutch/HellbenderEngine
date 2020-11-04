#pragma once

#include "functional"
template<typename Return, typename Object, typename... Args>
std::function<Return(Args...)> Attach(Return (Object::*function)(Args...), Object* instance)
{
    return [instance,function](Args... args)->Return { return (instance->*function)(args...); };
};