
#include "VK_CommandPool.h"
#include "core/utility/Log.h"
#include "VK_Device.h"
#include "VK_Swapchain.h"

namespace HBE {
    HBE::VK_CommandPool::~VK_CommandPool() {
        vkDestroyCommandPool(device->getHandle(), handle, nullptr);
    }

    void HBE::VK_CommandPool::clear() {
        vkFreeCommandBuffers(device->getHandle(), handle, static_cast<uint32_t>(command_buffers.size()),
                             command_buffers.data());
        command_buffers.clear();
    }

    VK_CommandPool::VK_CommandPool(const VK_Device *device, int command_buffers_count) {
        this->device = device;
        QueueFamilyIndices queueFamilyIndices = device->getPhysicalDevice().getQueueFamilyIndices();

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphics_family.value();

        poolInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
                         VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        if (vkCreateCommandPool(device->getHandle(), &poolInfo, nullptr, &handle) != VK_SUCCESS) {
            Log::error("failed to create command pool!");
        }
        createCommandBuffers(command_buffers_count);
    }

    void HBE::VK_CommandPool::createCommandBuffers(int count) {
        command_buffers.resize(count);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = handle;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) command_buffers.size();

        if (vkAllocateCommandBuffers(device->getHandle(), &allocInfo, command_buffers.data()) != VK_SUCCESS) {
            Log::error("failed to allocate command buffers!");
        }
    }


    void HBE::VK_CommandPool::begin(uint32_t i) const {
        current = i;
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(command_buffers[i], &beginInfo) != VK_SUCCESS) {
            Log::error("failed to begin recording command buffer!");
        }
    }

    void HBE::VK_CommandPool::end(uint32_t i) const {
        if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS) {
            Log::error("failed to record command buffer!");
        }
    }

    const std::vector<VkCommandBuffer> &HBE::VK_CommandPool::getBuffers() const {
        return command_buffers;
    }



    void VK_CommandPool::reset(uint32_t i) {
        vkResetCommandBuffer(command_buffers[i], 0);
    }

    const VkCommandBuffer &VK_CommandPool::getCurrentBuffer() const {
        return command_buffers[current];
    }

    const VkCommandPool &VK_CommandPool::getHandle() const {
        return handle;
    }


}
