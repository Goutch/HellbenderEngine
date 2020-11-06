#pragma once

#include <string>
#include <lua.hpp>
#include <vector>
#include "core/utility/Log.h"
class Deserializer {
private:
    lua_State* L;
public:
    Deserializer(std::string path);
    ~Deserializer();
    template<typename T>
    T get(std::string name);

    template<typename T>
    std::vector<T> getArray(std::string name);
};
