#pragma once


#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "Core.h"
#include "string"

using namespace glm;
namespace HBE {
    class HB_API Serializer {
    public:
        Serializer() {};

        ~Serializer() {};

        virtual void begin(const std::string &name) = 0;

        virtual void end() = 0;

        virtual void beginArray(const std::string &name) = 0;

        virtual void endArray() = 0;

        virtual void addField(const std::string &name, const std::string &value) = 0;

        virtual void addField(const std::string &name, int value) = 0;

        virtual void addField(const std::string &name, float value) = 0;

        virtual void addField(const std::string &name, bool value) = 0;

        virtual void addField(const std::string &name, const vec3 &value) = 0;

        virtual void addField(const std::string &name, const vec2 &value) = 0;

        virtual void addField(const std::string &name, const vec4 &value) = 0;

        virtual void addField(const std::string &name, const mat4 &value) = 0;

        virtual void addField(const std::string &name, const quat &value) = 0;
    };
}