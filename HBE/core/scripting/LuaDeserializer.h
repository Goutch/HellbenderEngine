#pragma once

#include <string>

#include <vector>
#include "core/utility/Log.h"
#include "core/serialization/Deserializer.h"
#include "Core.h"
class lua_State;
namespace HBE {
    class HB_API LuaDeserializer : Deserializer {
    private:
        int current_index = -1;
        lua_State *L;
    public:
        LuaDeserializer(std::string path);

        ~LuaDeserializer();

    private:
        void getObject(const std::string &name) override;

        void popObject() override;

        void popArray() override;

        mat4 getMat4(const std::string &name) override;

        vec3 getVec3(const std::string &name) override;

        vec2 getVec2(const std::string &name) override;

        float getFloat(const std::string &name) override;

        int getInt(const std::string &name) override;

        std::string getString(const std::string &name) override;
    };
}