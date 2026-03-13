#pragma once

#include "core/interface/MeshInterface.h"
#include "vector"
#include "vulkan/vulkan.h"
#include "VK_CommandPool.h"
#include "VK_StorageBuffer.h"

namespace HBE
{
    class VK_Device;

    class VK_Buffer;

    class VK_Renderer;

    class VK_Mesh
    {
        VK_Context* context = nullptr;

        std::vector<VK_Buffer> buffers;
        VkBufferUsageFlags extra_usages = 0;
        mutable bool bound = false;
        MeshInfo info;
        VK_Buffer indices_buffer;
        std::vector<VertexAttributeInfo> attributes_locations;
        uint32_t instance_count = 0;
        uint32_t vertex_count = 0;
        INDICES_TYPE indices_type;

        uint32_t indices_count = 0;

    public:
        void alloc(VK_Context* context, const MeshInfo& info);
        void release();
        VK_Mesh() = default;

        ~VK_Mesh();

        void setVertexIndices(const uint32_t* data, size_t count);

        void setVertexIndices(const uint16_t* data, size_t count);

        void setVertexIndices(const void* data, size_t count, size_t element_size);

        void setBuffer(uint32_t location, const void* vertices, size_t count);

        void setInstanceBuffer(uint32_t location, const void* data, size_t count);

        const VK_Buffer& getBuffer(uint32_t binding) const;

        const VK_Buffer& getIndicesBuffer() const;

        uint32_t getIndicesCount() const;
        uint32_t getVertexCount() const;
        uint32_t getInstanceCount() const;
        void bind() const;

        void unbind() const;

	    VkDeviceSize getAttributeElementSize(uint32_t location);

	    INDICES_TYPE getIndicesType();
    };
}
