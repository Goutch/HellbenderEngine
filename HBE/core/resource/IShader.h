#pragma once

#include "string"
#include "vector"
namespace HBE {
    enum class SHADER_TYPE {
        COMPUTE, VERTEX, FRAGMENT, GEOMETRY, NONE
    };

    class IShader {
    public:
        virtual ~IShader() = default;

        virtual void setSource(const std::vector<char> &source, SHADER_TYPE type) = 0;
    };
}

