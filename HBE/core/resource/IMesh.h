#pragma once
#include "vector"
#include "glm/glm.hpp"
using namespace glm;
namespace HBE{
    class IMesh
    {
    public:
        virtual ~IMesh() = default;
        virtual void setIndices(const std::vector<unsigned int> &data) = 0;

        virtual void setBuffer(unsigned int position, const std::vector<int> &data) = 0;

        virtual void setBuffer(unsigned int position, const std::vector<float> &data) = 0;

        virtual void setBuffer(unsigned int position, const std::vector<vec2> &data) = 0;

        virtual void setBuffer(unsigned int position, const std::vector<vec3> &data) = 0;

        virtual void setBuffer(unsigned int position, const std::vector<vec4> &data) = 0;

        virtual void setBuffer(unsigned int position, const std::vector<unsigned int> &data) = 0;

        virtual void setInstancedBuffer(unsigned int position, const std::vector<mat4> &data) = 0;

        virtual void bind() const = 0;

        virtual void unbind() const = 0;
    };
}
