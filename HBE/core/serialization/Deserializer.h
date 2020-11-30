#pragma once

#include "glm/glm.hpp"

#include "Core.h"

using namespace glm;
namespace HBE {
    class HB_API Deserializer {

    public:
        virtual void getObject(const std::string &name) = 0;

        virtual void popObject() = 0;

        virtual int getArray(const std::string &name) = 0;

        virtual void popArray() = 0;

        virtual mat4 getMat4(const std::string &name) = 0;

        virtual vec4 getVec4(const std::string &name) = 0;

        virtual vec3 getVec3(const std::string &name) = 0;

        virtual vec2 getVec2(const std::string &name) = 0;

        virtual float getFloat(const std::string &name) = 0;

        virtual int getInt(const std::string &name) = 0;

        virtual std::string getString(const std::string &name) = 0;

        virtual std::string getNext() = 0;

        virtual int getSize() = 0;
    };
}