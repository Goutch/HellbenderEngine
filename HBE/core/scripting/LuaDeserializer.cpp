//
// Created by user on 11/6/20.
//

#include "LuaDeserializer.h"
#include <lua.hpp>
LuaDeserializer::LuaDeserializer(std::string path) {
    L = luaL_newstate();

}

LuaDeserializer::~LuaDeserializer() {
    lua_close(L);
}

template<typename T>
std::vector<T> LuaDeserializer::getArray(const std::string& name) {
    std::vector<T> ts;

    lua_getfield(L, -1, name.c_str());

    return ts;
}

template<typename T>
T LuaDeserializer::get(const std::string& name) {
    std::vector<T> ts;
    lua_getfield(L, -1, name.c_str());
    if (lua_istable(L, -1)) {
        lua_pushnil(L);
        int key_index = -2;
        int value_index = -1;
        T *value;
        while (lua_next(L, key_index) != 0) {
            switch (lua_type(L, key_index)) {
                case LUA_TNUMBER://is unamed
                    printf("%g", lua_tonumber(L, key_index));
                    break;
                default:
                    Log::error(name+" data is not an array");
                    break;
            }
            value = static_cast<T>(lua_topointer(L, value_index));
            ts.push_back(*value);
            lua_pop(L, 1);
        }

    }
    return *ts;
}