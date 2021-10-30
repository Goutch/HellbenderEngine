#pragma once

#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"
#include "VK_PhysicalDevice.h"

namespace HBE {
    class VK_RenderPass;

    class VK_Swapchain;

    class VK_GraphicPipeline;

    class VK_CommandPool;

    class VK_Allocator;

    class VK_Device {
        VkDevice handle;

        VkQueue graphics_queue;
        VkQueue present_queue;
        VkQueue compute_queue;
        VkQueue transfer_queue;
        VK_PhysicalDevice *physical_device;
        VK_Allocator* allocator;
    public:
        VK_Device(VK_PhysicalDevice &physical_device);

        const VkDevice &getHandle() const;

        const VK_PhysicalDevice &getPhysicalDevice() const;

        const VkQueue &getGraphicsQueue() const;

        const VkQueue &getPresentQueue() const;

        const VkQueue &getComputeQueue() const;

        const VkQueue &getTransferQueue() const;

        VK_Allocator* getAllocator();

        void wait();

        ~VK_Device();
    };

}


