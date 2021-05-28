#pragma once

#include "vector"
#include "vulkan/vulkan.h"


namespace HBE {
    class VK_Device;
    class VK_Swapchain;
    class VK_CommandPool {
        VkCommandPool handle;
        const VK_Device *device;
        const VK_Swapchain* swapchain;
        std::vector<VkCommandBuffer> command_buffers;

        void createCommandBuffers();

    public:
        ~VK_CommandPool();

        VK_CommandPool(const VK_Device *device, const VK_Swapchain *swapchain);

        void begin(int i) const;

        void end(int i) const;


        const std::vector<VkCommandBuffer> &getBuffers() const;
    };
}
