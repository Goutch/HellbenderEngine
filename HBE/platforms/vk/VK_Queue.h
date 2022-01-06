#pragma once

#include "vulkan/vulkan.h"
#include "vector"

namespace HBE {
    class VK_Device;

    class VK_Fence;

    class VK_CommandPool;

    class VK_Fence;

    enum QUEUE_FAMILY {
        QUEUE_FAMILY_GRAPHICS,
        QUEUE_FAMILY_COMPUTE,
        QUEUE_FAMILY_PRESENT,
        QUEUE_FAMILY_TRANSFER,
    };

    class VK_Queue {
    private:
        VkQueue handle;
        VkDevice device_handle;
        VK_CommandPool *command_pool = nullptr;
        VK_Fence *fence;
        uint32_t family_index;
    public:
        VK_Queue(VK_Device *device, uint32_t family_index);
		~VK_Queue();
        const VkQueue &getHandle() const;

        void wait() const;

        void beginCommand();

        void endCommand();

        void submit();

        uint32_t getFamilyIndex();

        VK_CommandPool* getCommandPool();

        void submit(VkCommandBuffer const &command_buffer,
                    VkFence fence = VK_NULL_HANDLE,
                    VkSemaphore *wait = nullptr,
                    VkPipelineStageFlags *wait_stage = nullptr,
                    uint32_t wait_count = 0,
                    VkSemaphore *signal = nullptr,
                    uint32_t signal_count = 0) const;

    };
}



