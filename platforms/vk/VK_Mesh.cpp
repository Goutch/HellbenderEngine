#include <array>
#include "VK_Mesh.h"
#include "VK_Device.h"

#include "VK_Buffer.h"
#include "VK_Allocator.h"
#include "VK_Context.h"
#include "VK_Renderer.h"
#include "VK_StorageBuffer.h"
#include "core/Application.h"

namespace HBE
{
    VK_Mesh::VK_Mesh(VK_Context* context, const MeshInfo& info)
    {
        this->info = info;
        this->context = context;
        if ((info.flags & MESH_FLAG_USED_IN_RAYTRACING) != 0)
        {
            this->extra_usages |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
            this->extra_usages |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
        }
        if (info.flags & MESH_FLAG_USED_AS_STORAGE_BUFFER)
        {
            this->extra_usages |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        uint32_t max_location = 0;
        for (size_t i = 0; i < info.attribute_info_count; ++i)
        {
            if (info.attribute_infos[i].location > max_location)
                max_location = info.attribute_infos[i].location;
        }
        buffers.resize(max_location + 1);
        attributes_locations.resize(max_location + 1, {});

        for (size_t i = 0; i < info.attribute_info_count; ++i)
        {
            uint32_t location = info.attribute_infos[i].location;
            attributes_locations[location] = info.attribute_infos[i];
        }
    }

    void VK_Mesh::setBuffer(uint32_t location, const void* vertices, size_t count)
    {
        HB_ASSERT(location <= attributes_locations.size() &&
                  attributes_locations[location].location != ::std::numeric_limits<uint32_t>::max(),
                  "Trying to set buffer at a location that were not included in the attribute infos");

        this->vertex_count = count;
        if (count == 0)
        {
            Log::warning("Trying to set a mesh buffer with vertex count = 0");
            return;
        }
        VertexAttributeInfo& attribute_info = attributes_locations[location];
        VkDeviceSize buffer_size = attribute_info.size * count;

        //check if we need to release current buffer
        if (buffers[location].allocated() && buffer_size != buffers[location].getSize())
        {
            ReleaseRequest free_request{};
            free_request.vk_buffer = buffers[location].getHandle();
            free_request.allocation = buffers[location].getAllocation();
            free_request.fence = dynamic_cast<VK_Fence&>(*Application::instance->getContext()->getRenderer()->getLastFrameFence()).getHandle();
            context->allocator.releaseLater(free_request);
            buffers[location].reset();
        }

        if (!buffers[location].allocated())
        {
            BufferInfo buffer_info{};
            buffer_info.size = buffer_size;
            buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | extra_usages;
            buffer_info.memory_type_flags = attribute_info.preferred_memory_type_flags;
            buffers[location].alloc(context, buffer_info);
        }
        buffers[location].update(vertices);
    }


    void VK_Mesh::setInstanceBuffer(uint32_t location, const void* data, size_t count)
    {
        HB_ASSERT(location <= attributes_locations.size() &&
                  attributes_locations[location].location != ::std::numeric_limits<uint32_t>::max(),
                  "Trying to set buffer at a location that were not included in the attribute infos");
        this->instance_count = count;
        if (count == 0)
        {
            Log::warning("Trying to set a mesh buffer with instance count = 0");
            return;
        }
        setBuffer(location, data, count);
    }

    VK_Mesh::~VK_Mesh()
    {
        VK_Allocator* allocator = &context->allocator;
        for (size_t i = 0; i < buffers.size(); ++i)
        {
            if (buffers[i].allocated())
            {
                ReleaseRequest free_request{};
                free_request.vk_buffer = buffers[i].getHandle();
                free_request.allocation = buffers[i].getAllocation();
                VK_Fence* fence = dynamic_cast<VK_Fence*>(context->renderer.getCurrentFrameFence());
                free_request.fence = fence != nullptr ? fence->getHandle() : nullptr;
                allocator->releaseLater(free_request);
            }
            buffers[i].reset();
        }
        buffers.clear();
        if (indices_buffer.allocated())
        {
            ReleaseRequest free_request{};
            free_request.vk_buffer = indices_buffer.getHandle();
            free_request.allocation = indices_buffer.getAllocation();
            VK_Fence* fence = dynamic_cast<VK_Fence*>(context->renderer.getCurrentFrameFence());
            free_request.fence = fence->getHandle();
            allocator->releaseLater(free_request);
            indices_buffer.reset();
        }
    }

    void VK_Mesh::setVertexIndices(const void* data, size_t count, size_t element_size)
    {
        VkDeviceSize buffer_size = element_size * count;

        if (indices_buffer.allocated() && buffer_size != indices_buffer.getSize())
        {
            ReleaseRequest free_request{};
            free_request.vk_buffer = indices_buffer.getHandle();
            free_request.allocation = indices_buffer.getAllocation();
            context->allocator.releaseLater(free_request);
        }


        if (!indices_buffer.allocated())
        {
            BufferInfo buffer_info{};
            buffer_info.size = buffer_size;
            buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | extra_usages;
            buffer_info.memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
            indices_buffer.alloc(context, buffer_info);
        }

        indices_buffer.update(data);


        index_count = count;
    }

    void VK_Mesh::setVertexIndices(const uint16_t* data, size_t count)
    {
        indices_type = INDICES_TYPE_UINT16;
        setVertexIndices(data, count, sizeof(data[0]));
    }

    void VK_Mesh::setVertexIndices(const uint32_t* data, size_t count)
    {
        indices_type = INDICES_TYPE_UINT32;
        setVertexIndices(data, count, sizeof(data[0]));
    }

    void VK_Mesh::bind() const
    {
        if (bound)
            return;
        bound = true;

        HB_ASSERT(buffers.size() <= 16, "Too many mesh buffers : buffers.size() > 16");
        VkBuffer flat_buffers[16];
        VkDeviceSize offsets[16];

        int buffer_count = 0;
        for (const VK_Buffer& buffer : buffers)
        {
            flat_buffers[buffer_count] = buffer.getHandle();
            offsets[buffer_count] = 0;
            buffer_count++;
        }

        vkCmdBindVertexBuffers(context->renderer.getCommandPool()->getCurrentBuffer(),
                               0,
                               buffer_count,
                               flat_buffers,
                               offsets);
        if (indices_type != INDICES_TYPE_NONE)
        {
            vkCmdBindIndexBuffer(context->renderer.getCommandPool()->getCurrentBuffer(),
                                 indices_buffer.getHandle(),
                                 0,
                                 indices_type == INDICES_TYPE_UINT32 ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
        }
    }

    void VK_Mesh::unbind() const
    {
        bound = false;
    }

    const VK_Buffer& VK_Mesh::getBuffer(uint32_t binding) const
    {
        return buffers.at(binding);
    }

    const VK_Buffer& VK_Mesh::getIndicesBuffer() const
    {
        return indices_buffer;
    }

}
