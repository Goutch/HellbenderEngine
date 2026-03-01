#pragma once

#include "vulkan/vulkan.h"
#include "VK_CommandPool.h"

namespace HBE
{
    class VK_Device;

    class VK_Fence;

    class VK_CommandPool;

    class VK_Fence;

    enum QUEUE_FAMILY
    {
        QUEUE_FAMILY_GRAPHICS,
        QUEUE_FAMILY_COMPUTE,
        QUEUE_FAMILY_PRESENT,
        QUEUE_FAMILY_TRANSFER,
    };

    class VK_Queue
    {
    private:
        VkQueue handle;
        VkDevice device_handle;
        VK_CommandPool command_pool;
        uint32_t family_index;
        QUEUE_FAMILY queue_family;

    public:
        VK_Queue(VK_Device* device, QUEUE_FAMILY family, uint32_t family_index);

        ~VK_Queue();

        const VkQueue& getHandle() const;

        void wait() const;

        void beginCommand();

        void endCommand();

        VK_Fence& submitCommand();

        uint32_t getFamilyIndex() const;

        VK_CommandPool* getCommandPool();

        void submit(VkCommandBuffer const& command_buffer,
                    VkFence fence = VK_NULL_HANDLE,
                    VkSemaphore* wait = nullptr,
                    VkPipelineStageFlags* wait_stage = nullptr,
                    uint32_t wait_count = 0,
                    VkSemaphore* signal = nullptr,
                    uint32_t signal_count = 0) const;
    };
}
