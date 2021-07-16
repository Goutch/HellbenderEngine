#pragma once

#include "core/resource/Mesh.h"
#include "vector"
namespace HBE {
    class VK_Device;

    class VK_Mesh : public Mesh {
        const VK_Device *device;
    public:
        VK_Mesh(const VK_Device *device);

        ~VK_Mesh() = default;

        void setIndices(const std::vector<unsigned int> &data) override;

        void setVertices(void *vertices, size_t count,const VertexLayout *layout) override;

        void setBuffer(unsigned int position, const std::vector<int> &data) override;

        void setBuffer(unsigned int position, const std::vector<float> &data) override;

        void setBuffer(unsigned int position, const std::vector<vec2> &data) override;

        void setBuffer(unsigned int position, const std::vector<vec3> &data) override;

        void setBuffer(unsigned int position, const std::vector<vec4> &data) override;

        void setBuffer(unsigned int position, const std::vector<unsigned int> &data) override;

        void setInstancedBuffer(unsigned int position, const std::vector<mat4> &data) override;

        void bind() const override;

        void unbind() const override;

    };

}
