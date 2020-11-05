#pragma once

#include "lua.hpp"
#include "string"
#include "tuple"


class ScriptManager {
    static lua_State *L;
public:
    static void init();
    static void terminate();
    static void doCommand(std::string command);

    template<typename Return, typename... Args>
    static void registerFunction(Return (*function)(Args...),std::string function_name) {
        lua_register(L,function_name,[function](lua_State *L) ->int{

            std::tuple<Args...> args;
            int number_args=lua_gettop(L);
            for (size_t i = number_args; i >=0; --i) {
                args[i]=static_cast<typename std::tuple_element<i, std::tuple<Args...>>::type>(lua_topointer(L,i));
            }
            std::apply(function, args);
            return 1;
        });

    }
};
