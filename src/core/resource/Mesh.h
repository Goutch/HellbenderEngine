
#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "GLSL_TYPE.h"
using namespace glm;
class Mesh {
protected:
    unsigned int vertex_count = 0;
    unsigned int index_count = 0;
    bool has_index_buffer = false;
    unsigned int instance_count = 1;
public:
    virtual ~Mesh() {};

    virtual void setIndices(const std::vector<unsigned int> &data) = 0;

    virtual void setBuffer(unsigned int position, const std::vector<int> &data) = 0;

    virtual void setBuffer(unsigned int position, const std::vector<float> &data) = 0;

    virtual void setBuffer(unsigned int position, const std::vector<vec2> &data) = 0;

    virtual void setBuffer(unsigned int position, const std::vector<vec3> &data) = 0;

    virtual void setBuffer(unsigned int position, const std::vector<vec4> &data) = 0;

    virtual void setBuffer(unsigned int position, const std::vector<unsigned int> &data)=0;

    virtual void setInstancedBuffer(unsigned int position, const std::vector<mat4> &data) = 0;

    unsigned int getVertexCount() const { return vertex_count; }

    unsigned int getIndexCount() const { return index_count; }

    unsigned int getInstanceCount() const { return instance_count; }

    bool hasIndexBuffer() const { return has_index_buffer; }

    virtual void bind() const = 0;

    virtual void unbind() const = 0;
};

