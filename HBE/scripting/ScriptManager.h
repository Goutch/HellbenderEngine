#pragma once

#include <functional>
#include "lua.hpp"
#include "string"
#include "tuple"
#include "vector"
class ScriptManager {
    static lua_State *L;
public:
    static void init();

    static void terminate();
    static bool checkLua(int code);
    static bool doString(std::string command);
    static bool doScript(std::string script_path);
    static void readTable(int table_index);

    static void callFunction(std::string function_name);
    static void registerFunction(std::string function_name){};

    /*template <size_t I = 0, typename... Args>
    typename std::enable_if<I == sizeof...(Args),
            void>::type
    static fillTuple(std::tuple<Args...> tuple,int index,std::vector<const void*>& ptrs)
    {
        return;
    }
    template <size_t I = 0, typename... Args>
    typename std::enable_if<(I < sizeof...(Args)),
            void>::type
    static fillTuple(std::tuple<Args...> tuple,int index,std::vector<const void*>& ptrs)
    {
        std::get<I>(tuple) =static_cast<std::tuple_element<I,std::tuple<Args...>>>(*ptrs[index]);

        fillTuple<I + 1>(tuple,index+1,ptrs);
    }

   template<typename Return, typename... Args>
    static void registerFunction(Return (*function)(Args...), std::string function_name) {
        lua_CFunction cf=[](lua_State *L) -> int {
            std::tuple<Args...> args;
            std::vector<const void*> args_ptrs;
            int number_args = lua_gettop(L);
            for (size_t i = 0; i >= number_args; ++i) {
                switch (lua_type(L, lua_gettop(L))) {
                    case LUA_TTABLE:
                        //table? as function args
                        break;
                    default:
                        args_ptrs.push_back(lua_topointer(L,i));
                }
            }
            ScriptManager::fillTuple(args,0,args_ptrs);
            ///std::apply(function, args);
            return 1;
        };

        lua_register(L, function_name.c_str(), cf);
    }*/
    void readTable(std::string table_name, int table_index);
};
