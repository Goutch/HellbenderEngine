
#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "Core.h"


using namespace glm;
namespace HBE {
    class IMesh;

    class HB_API Mesh final {
        IMesh *instance;

        unsigned int vertex_count = 0;
        unsigned int index_count = 0;
        bool has_index_buffer = false;
        unsigned int instance_count = 1;
    public:
        Mesh();

        virtual ~Mesh();

        void setIndices(const std::vector<unsigned int> &data);

        void setBuffer(unsigned int position, const std::vector<int> &data);

        void setBuffer(unsigned int position, const std::vector<float> &data);

        void setBuffer(unsigned int position, const std::vector<vec2> &data);

        void setBuffer(unsigned int position, const std::vector<vec3> &data);

        void setBuffer(unsigned int position, const std::vector<vec4> &data);

        void setBuffer(unsigned int position, const std::vector<unsigned int> &data);

        void setInstancedBuffer(unsigned int position, const std::vector<mat4> &data);

        unsigned int getVertexCount() const;

        unsigned int getIndexCount() const;

        unsigned int getInstanceCount() const;

        bool hasIndexBuffer() const;

        virtual void bind() const;

        virtual void unbind() const;

    };
}

