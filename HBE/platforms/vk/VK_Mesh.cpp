
#include <array>
#include "VK_Mesh.h"
#include "VK_Device.h"
#include "VK_VertexLayout.h"

namespace HBE {
    VK_Mesh::VK_Mesh(const VK_Device *device, const VK_CommandPool *command_pool) {
        this->device = device;
        this->command_pool = command_pool;
    }

    void VK_Mesh::setVertices(int position, void *vertices, size_t count, const VertexLayout *layout) {
        this->vertex_count = count;

        if (positions.find(position) != positions.end()) {
            vkDestroyBuffer(device->getHandle(), buffers[position], nullptr);
            vkFreeMemory(device->getHandle(), memories[position], nullptr);
            buffers.erase(position);
            memories.erase(position);
            positions.erase(position);
        }

        positions.emplace(position);
        buffers.emplace(position, VkBuffer());
        memories.emplace(position,VkDeviceMemory());

        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = layout->getBytesPerVertex() * count;
        buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(device->getHandle(), &buffer_info, nullptr, &buffers[position]) != VK_SUCCESS) {
            Log::error("failed to create vertex buffer!");
        }
        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(device->getHandle(), buffers[position], &memory_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = memory_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(memory_requirements.memoryTypeBits,
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(device->getHandle(), &alloc_info, nullptr, &memories[position]) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }
        vkBindBufferMemory(device->getHandle(), buffers[position], memories[position], 0);
        void *data;
        vkMapMemory(device->getHandle(), memories[position], 0, buffer_info.size, 0, &data);
        memcpy(data, vertices, (size_t) buffer_info.size);
        vkUnmapMemory(device->getHandle(), memories[position]);
    }

    uint32_t VK_Mesh::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memory_propeties;
        vkGetPhysicalDeviceMemoryProperties(device->getPhysicalDevice().getHandle(), &memory_propeties);
        for (uint32_t i = 0; i < memory_propeties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memory_propeties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        Log::error("Failed to find suitable memory type");
        return 0;
    }

    VK_Mesh::~VK_Mesh() {
        Log::message("destroy vk_mesh");
        for (auto pos: positions) {
            vkDestroyBuffer(device->getHandle(), buffers[pos], nullptr);
            vkFreeMemory(device->getHandle(), memories[pos], nullptr);
        }
        buffers.clear();
        memories.clear();
        positions.clear();
    }

    void VK_Mesh::setIndices(const std::vector<unsigned int> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<int> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<float> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<vec2> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<vec3> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<vec4> &data) {

    }

    void VK_Mesh::setBuffer(unsigned int position, const std::vector<unsigned int> &data) {

    }

    void VK_Mesh::setInstancedBuffer(unsigned int position, const std::vector<mat4> &data) {

    }

    void VK_Mesh::bind() const {

        VkBuffer* flat_buffers=new VkBuffer[positions.size()];
        VkDeviceSize* offsets=new VkDeviceSize[positions.size()];
        int i=0;
        for (auto position: positions) {
            flat_buffers[i]=buffers[position];
            offsets[i]=0;
            i++;
        }

        vkCmdBindVertexBuffers(command_pool->getCurrentBuffer(), 0, positions.size(), flat_buffers, offsets);
    }

    void VK_Mesh::unbind() const {

    }


}



