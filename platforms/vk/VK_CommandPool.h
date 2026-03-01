#pragma once

#include "vector"
#include "vulkan/vulkan.h"
#include "VK_Fence.h"
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
        std::vector<VK_Fence> fences;

        void begin(uint32_t i) const;

        void end(uint32_t i) const;

        void reset(uint32_t i);

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

        void wait(int frame);

        void waitAll();

        VkCommandPool getHandle() const;

        VK_Fence& submit(VK_Queue& queue, VkSemaphore* wait = nullptr,
                         VkPipelineStageFlags* wait_stage = nullptr,
                         uint32_t wait_count = 0,
                         VkSemaphore* signal = nullptr,
                         uint32_t signal_count = 0);


        VK_Fence& getCurrentFence();

        VK_Fence& getLastFence();
    };
}
