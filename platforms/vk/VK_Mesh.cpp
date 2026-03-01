#include <array>
#include "VK_Mesh.h"
#include "VK_Device.h"

#include "VK_Buffer.h"
#include "VK_Allocator.h"
#include "VK_Renderer.h"
#include "VK_StorageBuffer.h"
#include "core/Application.h"

namespace HBE
{
    VK_Mesh::VK_Mesh(VK_Renderer* renderer,  const MeshInfo& info)
    {
        this->info = info;
        this->renderer = renderer;
        this->device = renderer->getDevice();
        this->command_pool = renderer->getCommandPool();
        if ((info.flags & MESH_FLAG_USED_IN_RAYTRACING) != 0)
        {
            this->extra_usages |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
            this->extra_usages |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
        }
        if (info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER)
        {
            this->extra_usages |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        uint32_t max_location = 0;
        for (size_t i = 0; i < info.attribute_info_count; ++i)
        {
            if (info.attribute_infos[i].location > max_location)
                max_location = info.attribute_infos[i].location;
        }
        buffers.resize(max_location + 1, nullptr);
        if (info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER)
            storage_buffers.resize(max_location + 1, nullptr);
        attributes_locations.resize(max_location + 1, {});

        for (size_t i = 0; i < info.attribute_info_count; ++i)
        {
            uint32_t location = info.attribute_infos[i].location;
            buffers[location] = nullptr;
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

        //check if a new buffer is required
        if (buffers[location] && buffer_size != buffers[location]->getSize())
        {
            FreeRequest free_request{};
            free_request.buffer = buffers[location];
            free_request.fence = static_cast<VK_Fence*>(Application::instance->getContext()->getRenderer()->getCurrentFrameFence())->getHandle();
            device->getAllocator()->freeLater(free_request);
            buffers[location] = nullptr;
            if ((info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER) && storage_buffers[location] != nullptr)
            {
                free_request = {};
                free_request.storage_buffer = storage_buffers[location];
                free_request.fence = static_cast<VK_Fence*>(Application::instance->getContext()->getRenderer()->getCurrentFrameFence())->getHandle();
                device->getAllocator()->freeLater(free_request);
            }
        }

        if (buffers[location] != nullptr)
        {
            buffers[location]->update(vertices, buffer_size);
        }
        else
        {
            buffers[location] = new VK_Buffer(device,
                                              vertices,
                                              buffer_size,
                                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | extra_usages,
                                              attribute_info.preferred_memory_type_flags);
            if (info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER)
            {
                if (storage_buffers.size() < location)
                    storage_buffers.resize(location + 1, nullptr);
                storage_buffers[location] = new VK_StorageBuffer(device, buffers[location], attribute_info.size,
                                                                 count, false);
            }
        }
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
        VK_Allocator* allocator = device->getAllocator();
        for (size_t i = 0; i < buffers.size(); ++i)
        {
            if (buffers[i])
            {
                FreeRequest free_request{};
                free_request.buffer = buffers[i];
                VK_Fence* fence = static_cast<VK_Fence*>(Application::instance->getContext()->getRenderer()->getCurrentFrameFence());
                free_request.fence = fence != nullptr ? fence->getHandle() : nullptr;
                allocator->freeLater(free_request);
            }
            buffers[i] = nullptr;
        }
        buffers.clear();
        if (indices_buffer)
        {
            FreeRequest free_request{};
            free_request.buffer = indices_buffer;
            VK_Fence* fence = static_cast<VK_Fence*>(Application::instance->getContext()->getRenderer()->getCurrentFrameFence());
            free_request.fence = fence != nullptr ? fence->getHandle() : nullptr;
            allocator->freeLater(free_request);
            indices_buffer = nullptr;
        }

        if (indices_storage_buffer)
        {
            FreeRequest free_request{};
            free_request.storage_buffer = indices_storage_buffer;
            VK_Fence* fence = static_cast<VK_Fence*>(Application::instance->getContext()->getRenderer()->getCurrentFrameFence());
            free_request.fence = fence != nullptr ? fence->getHandle() : nullptr;
            allocator->freeLater(free_request);
            indices_storage_buffer = nullptr;
        }
        for (uint32_t i = 0; i < storage_buffers.size(); i++)
        {
            FreeRequest free_request{};
            free_request.storage_buffer = storage_buffers[i];
            VK_Fence* fence = static_cast<VK_Fence*>(Application::instance->getContext()->getRenderer()->getCurrentFrameFence());
            free_request.fence = fence != nullptr ? fence->getHandle() : nullptr;
            allocator->freeLater(free_request);
        }
        storage_buffers.clear();
    }

    void VK_Mesh::setVertexIndices(const void* data, size_t count, size_t element_size)
    {
        if (indices_buffer)
        {
            FreeRequest free_request{};
            free_request.buffer = indices_buffer;
            VK_Fence* fence = static_cast<VK_Fence*>(Application::instance->getContext()->getRenderer()->getCurrentFrameFence());
            free_request.fence = fence != nullptr ? fence->getHandle() : nullptr;
            device->getAllocator()->freeLater(free_request);
        }
        VkDeviceSize buffer_size = element_size * count;


        indices_buffer = new VK_Buffer(device,
                                       data,
                                       buffer_size,
                                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT | extra_usages,
                                       MEMORY_TYPE_FLAG_NONE);

        if (info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER)
        {
            if (indices_storage_buffer)
            {
                FreeRequest free_request{};
                free_request.storage_buffer = indices_storage_buffer;
                VK_Fence* fence = static_cast<VK_Fence*>(Application::instance->getContext()->getRenderer()->getCurrentFrameFence());
                free_request.fence = fence != nullptr ? fence->getHandle() : nullptr;
                device->getAllocator()->freeLater(free_request);
            }
            indices_storage_buffer = new VK_StorageBuffer(device, indices_buffer, element_size, count, false);
        }
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
        for (auto buffer : buffers)
        {
            flat_buffers[buffer_count] = buffer->getHandle();
            offsets[buffer_count] = 0;
            buffer_count++;
        }

        vkCmdBindVertexBuffers(command_pool->getCurrentBuffer(),
                               0,
                               buffer_count,
                               flat_buffers,
                               offsets);
        if (indices_type != INDICES_TYPE_NONE)
        {
            vkCmdBindIndexBuffer(command_pool->getCurrentBuffer(),
                                 indices_buffer->getHandle(),
                                 0,
                                 indices_type == INDICES_TYPE_UINT32 ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
        }
    }

    void VK_Mesh::unbind() const
    {
        bound = false;
    }

    const VK_Buffer* VK_Mesh::getBuffer(uint32_t binding) const
    {
        return buffers.at(binding);
    }

    const VK_Buffer* VK_Mesh::getIndicesBuffer() const
    {
        return indices_buffer;
    }

    StorageBuffer* VK_Mesh::getAttributeStorageBuffer(uint32_t location) const
    {
        HB_ASSERT(info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER,
                  "Trying to get a storage buffer from a mesh that does not have the flag MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER");

        auto storageBuffer = storage_buffers[location];
        HB_ASSERT(storageBuffer != nullptr,
                  "Trying to get a storage buffer from a mesh that does not have an attribute at location:" + std::
                  to_string(location));

        return storageBuffer;
    }

    StorageBuffer* VK_Mesh::getIndicesStorageBuffer() const
    {
        HB_ASSERT(info.flags & MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER,
                  "Trying to get a storage buffer from a mesh that does not have the flag MESH_FLAG_GENERATE_ATTRIBUTE_STORAGE_BUFFER");
        HB_ASSERT(hasIndexBuffer(), "Trying to get a storage buffer from a mesh that does not have an index buffer");
        return indices_storage_buffer;
    }
}
