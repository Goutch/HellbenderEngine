#pragma once

#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"
#include "VK_PhysicalDevice.h"

namespace HBE {
    class VK_RenderPass;

    class VK_Swapchain;

    class VK_GraphicPipeline;

    class VK_CommandPool;

    class VK_Device {
        VkDevice handle;

        VkQueue graphics_queue;
        VkQueue present_queue;
        VK_PhysicalDevice *physical_device;
    public:
        VK_Device(VK_PhysicalDevice &physical_device);

        VK_Swapchain &getSwapchain();

        VK_GraphicPipeline &getGraphicPipeline();

        VK_RenderPass &getRenderPass();

        const VkDevice &getHandle() const;

        VK_PhysicalDevice &getPhysicalDevice();

        ~VK_Device();
    };

}


