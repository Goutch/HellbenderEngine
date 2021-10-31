#pragma once
#include "vulkan/vulkan.h"
#include "vector"
namespace HBE{
    class VK_Device;
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
        VK_Device* device;
        uint32_t max_opp_in_flight;
        std::vector<VK_Fence*> fences;
        mutable uint32_t current_opp=0;
    public:
        VK_Queue(VK_Device* device,uint32_t family_index,uint32_t max_opp_in_flight);


        const VkQueue& getHandle() const;
        void wait() const;
        void
        submit(VkCommandBuffer const &command_buffer,
               VkSemaphore *wait,
               VkPipelineStageFlags *wait_stage,
               uint32_t wait_count,
               VkSemaphore *signal, uint32_t signal_count) const;
    };
}



