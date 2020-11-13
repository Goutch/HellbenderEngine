#pragma once

#include <core/serialization/Serializer.h>
#include "string"

class EditorSerializer :Serializer{
    void begin(const std::string &name) ;

    void end() override;

    void beginArray(const std::string &name);

    void endArray() override;

    void addField(const std::string &name, const std::string &value) ;

    void addField(const std::string &name, int value);

    void addField(const std::string &name, float value);

    void addField(const std::string &name, bool value);

    void addField(const std::string &name, vec3 value);

    void addField(const std::string &name, vec2 value);

    void addField(const std::string &name, vec4 value);

    void addField(const std::string &name, quat value);
};
