#pragma once

#include "core/resource/Mesh.h"
#include "vector"
#include "vulkan/vulkan.h"
#include "set"
#include "VK_CommandPool.h"

namespace HBE {
    class VK_Device;
    class VK_Buffer;
    class VK_Mesh : public Mesh {
        const VK_CommandPool *command_pool;
        VK_Device *device;
        mutable std::unordered_map<unsigned int,VK_Buffer*> buffers;

    public:
        VK_Mesh(VK_Device *device, const VK_CommandPool *command_pool);

        ~VK_Mesh() override;

        void setIndices(const std::vector<unsigned int> &data) override;

        void setVertices(int position, void *vertices, size_t count, const VertexLayout *layout) override;

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
