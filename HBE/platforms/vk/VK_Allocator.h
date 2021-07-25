#pragma once

#include "vulkan/vulkan.h"
#include "vector"
#include "list"
#include "unordered_map"

namespace HBE {
    class VK_CommandPool;
    class VK_Buffer;
    class VK_Device;
    struct Block;
    struct Allocation {
        const VkDeviceSize size;
        const VkDeviceSize offset;
        Block& block;
		std::list<Allocation>::iterator position_in_block;
    };
    struct Block {
        VkDeviceSize size;
        std::list<Allocation> allocations;
        VkDeviceMemory memory=VK_NULL_HANDLE;
        uint32_t memory_type;
        uint32_t index;
        uint32_t alloc_count=0;
        VkDeviceSize remaining;
    };

    class VK_Allocator {
        VK_CommandPool *command_pool;
        const VK_Device *device;
        const VkDeviceSize BLOCK_SIZE = 134217728; //1024*1024*128 = 128mb
        std::unordered_map<uint32_t,std::vector<Block>> blocks;
		VkPhysicalDeviceMemoryProperties memory_propeties;

    public:

        VK_Allocator(const VK_Device *device);

        ~VK_Allocator();

        Allocation& alloc(VkBuffer &buffer, VkDeviceSize size,VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

        void free(VkBuffer &buffer,Allocation& allocation);
        void copy(VK_Buffer *src, VK_Buffer *dest);

    private:
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        Allocation& getAllocation(VkBuffer& buffer,VkDeviceSize size, VkMemoryPropertyFlags properties);
    };
}

