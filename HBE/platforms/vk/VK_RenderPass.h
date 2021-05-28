#pragma once
#include "vulkan/vulkan.h"


namespace HBE{
    class VK_Device;

    class VK_RenderPass {
        VkRenderPass handle;
        const VK_Device* device;
    public:

        ~VK_RenderPass();
        VK_RenderPass(const VK_Device *device);
        void begin(const VkCommandBuffer& command_buffer, const VkFramebuffer& framebuffer,const VkExtent2D& extent) const;
        void end(const VkCommandBuffer& command_buffer) const;
        const VkRenderPass& getHandle() const;
    };
}

