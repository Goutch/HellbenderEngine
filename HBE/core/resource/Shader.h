#pragma once

#include "Core.h"
#include "string"

namespace HBE {
    enum class SHADER_TYPE {
        COMPUTE, VERTEX, FRAGMENT, GEOMETRY
    };

    class HB_API Shader {
    protected:
        SHADER_TYPE type;

        Shader(SHADER_TYPE type);

    public:
        static std::string getSource(const std::string &path);

        static Shader *create(SHADER_TYPE type, const std::string &source);
        SHADER_TYPE getType();
        virtual ~Shader() {};
    };
}

