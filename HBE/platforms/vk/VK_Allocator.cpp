
#include "VK_Allocator.h"
#include "VK_CommandPool.h"
#include "VK_Buffer.h"
#include "VK_Device.h"

namespace HBE {
    VK_Allocator::VK_Allocator(const VK_Device *device) {
        this->device = device;
        this->command_pool = new VK_CommandPool(device, 1);
    }

    VK_Allocator::~VK_Allocator() {
        delete command_pool;
        for (auto it = blocks.begin(); it != blocks.end(); ++it) {
            for (Block block:it->second) {
                vkFreeMemory(device->getHandle(), block.memory, nullptr);
            }
        }
    }

    uint32_t VK_Allocator::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
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

    //https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkMemoryPropertyFlagBits.html
    Allocation &VK_Allocator::alloc(VkBuffer &buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


        if (vkCreateBuffer(device->getHandle(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            Log::error("failed to create buffer!");
        }

        Allocation &allocation = getAllocation(buffer, size, properties);
        vkBindBufferMemory(device->getHandle(), buffer, allocation.block.memory, allocation.offset);
        return allocation;
    }

    void VK_Allocator::copy(VK_Buffer *src, VK_Buffer *dest) {
        command_pool->begin(0);
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = src->getSize();
        vkCmdCopyBuffer(command_pool->getCurrentBuffer(), src->getHandle(), dest->getHandle(), 1, &copyRegion);

        command_pool->end(0);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command_pool->getCurrentBuffer();

        //todo use transfer queue
        vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(device->getGraphicsQueue());
    }


    Allocation &VK_Allocator::getAllocation(VkBuffer &buffer, VkDeviceSize size, VkMemoryPropertyFlags properties) {
        VkDeviceSize position = 0;
        if (blocks.find(properties) == blocks.end())
            blocks.emplace(properties, std::vector<Block>());
        for (auto block_it = blocks[properties].begin(); block_it != blocks[properties].end(); ++block_it) {
            for (auto allocation_it = block_it->allocations.begin(); allocation_it != block_it->allocations.end(); ++allocation_it) {
                if (allocation_it->offset - position <= size) {
                    block_it->alloc_count++;
                    return *block_it->allocations.emplace(allocation_it, Allocation{
                            .size=size,
                            .offset=0,
                            .block=*block_it});
                } else {
                    position = allocation_it->offset + allocation_it->size;
                }
            }
            if (block_it->size - position <= size) {
                block_it->alloc_count++;
                return block_it->allocations.emplace_back(Allocation{
                        .size=size,
                        .offset=0,
                        .block=*block_it});
            }
        }

        uint32_t index = blocks[properties].size();
        Block &block = blocks[properties].emplace_back(Block{
                .size= size > BLOCK_SIZE ? size : BLOCK_SIZE,
                .allocations=std::list<Allocation>(),
                .memory=VK_NULL_HANDLE,
                .property_flags=properties,
                .index=index,
                .alloc_count=0});

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device->getHandle(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = block.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device->getHandle(), &allocInfo, nullptr, &block.memory) != VK_SUCCESS) {
            Log::error("failed to allocate buffer memory!");
        }
        block.alloc_count++;
        return block.allocations.emplace_back(Allocation{
                .size=size,
                .offset=0,
                .block=block});
    }

    void VK_Allocator::free(VkBuffer &buffer, Allocation &allocation) {

        vkDestroyBuffer(device->getHandle(), buffer, nullptr);
        allocation.block.alloc_count--;
        VkMemoryPropertyFlags flags = allocation.block.property_flags;
        if (allocation.block.alloc_count == 0) {
            vkFreeMemory(device->getHandle(), allocation.block.memory, nullptr);

            auto it = blocks[flags].erase(blocks[flags].begin() + allocation.block.index);

            for (; it != blocks[flags].end(); ++it) {
                it->index--;
            }

        }

    }
}
