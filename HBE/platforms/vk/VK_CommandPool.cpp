
#include "VK_CommandPool.h"
#include "core/utility/Log.h"
#include "VK_Device.h"
#include "VK_Swapchain.h"
namespace HBE {
    HBE::VK_CommandPool::~VK_CommandPool() {
        vkDestroyCommandPool(device->getHandle(), handle, nullptr);
    }
    VK_CommandPool::VK_CommandPool(const VK_Device *device, int n) {
        this->device = device;
        QueueFamilyIndices queueFamilyIndices = device->getPhysicalDevice().getQueueFamilyIndices();

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphics_family.value();
        poolInfo.flags = 0; // Optional

        if (vkCreateCommandPool(device->getHandle(), &poolInfo, nullptr, &handle) != VK_SUCCESS) {
            Log::error("failed to create command pool!");
        }
        createCommandBuffers(n);
    }

    void HBE::VK_CommandPool::createCommandBuffers(int n) {
        command_buffers.resize(n);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = handle;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) command_buffers.size();

        if (vkAllocateCommandBuffers(device->getHandle(), &allocInfo, command_buffers.data()) != VK_SUCCESS) {
            Log::error("failed to allocate command buffers!");
        }
    }


    void HBE::VK_CommandPool::begin(int i) const {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(command_buffers[i], &beginInfo) != VK_SUCCESS) {
            Log::error("failed to begin recording command buffer!");
        }
    }

    void HBE::VK_CommandPool::end(int i) const {
        if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS) {
            Log::error("failed to record command buffer!");
        }
    }

    const std::vector<VkCommandBuffer> &HBE::VK_CommandPool::getBuffers() const {
        return command_buffers;
    }


}

