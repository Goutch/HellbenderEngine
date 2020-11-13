#pragma once

#include <string>

#include <vector>
#include "core/utility/Log.h"
#include "core/serialization/Deserializer.h"
#include "Core.h"
class lua_State;

class HB_API LuaDeserializer:Deserializer {
private:
    lua_State* L;
public:
    LuaDeserializer(std::string path);
    ~LuaDeserializer();
    template<typename T>
    T get(const std::string& name);

    template<typename T>
    std::vector<T> getArray(const std::string& name);
};
