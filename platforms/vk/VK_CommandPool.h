#pragma once

#include "vector"
#include "vulkan/vulkan.h"
#include "HBE/platforms/vk/resources/VK_Fence.h"
#include "memory"

namespace HBE
{
    class VK_Device;

    class VK_Swapchain;
    class VK_Queue;

    class VK_CommandPool
    {
        mutable int32_t current = 0;
        int32_t last_summited = 0;
        VkCommandPool handle;
        VK_Context* context;
        std::vector<VkCommandBuffer> command_buffers;
        std::vector<FenceHandle> fences;

        void reset();

    public:
        void init(VK_Context* context, uint32_t command_buffers_count, const VK_Queue& queue);

        void release();

        VK_CommandPool() = default;

        VK_CommandPool(const VK_CommandPool&) = delete;

        VK_CommandPool& operator=(const VK_CommandPool&) = delete;

        ~VK_CommandPool() = default;

        void begin() const;

        void end() const;

        const std::vector<VkCommandBuffer>& getBuffers() const;

        const VkCommandBuffer& getCurrentBuffer() const;

        void clear();

        void createCommandBuffers(int count);
        void waitAll();

        VkCommandPool getHandle() const;

        FenceHandle submit(VK_Queue& queue,
                           VkSemaphore* wait = nullptr,
                           VkPipelineStageFlags* wait_stage = nullptr,
                           uint32_t wait_count = 0,
                           VkSemaphore* signal = nullptr,
                           uint32_t signal_count = 0);


        FenceHandle getCurrentFence();

        FenceHandle getLastFence();

        uint32_t getCommandBufferIndex() const;
    };
}
