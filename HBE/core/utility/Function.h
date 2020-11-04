#pragma once
#include <functional>

template<typename Return,typename Object, typename... Args>
class Function {
    std::function<Return (Args...)> function;

    template<typename Object>
    void bind(Object* object,Return(Object::* fun)(Args...)){
        function = [*object](TArgs... args){return ((*object).*f)(args...);};
    }

    void bind(Function f){
        functionstd::bind(f);
    }

    Return execute(Args... args)
    {
        return function(args...);
    }
};