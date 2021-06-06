#pragma once
#include "vulkan/vulkan.h"
#include "vector"

namespace HBE{
    class VK_Device;
    class VK_Swapchain;
    class VK_RenderPass {
        VkRenderPass handle;
        const VK_Device* device;
        const VK_Swapchain *swapchain;
        VkExtent2D extent;
        std::vector<VkFramebuffer> frame_buffers;
    public:
        ~VK_RenderPass();
        VK_RenderPass(const VK_Device *device,const VK_Swapchain* swapchain);
        void begin(const VkCommandBuffer& command_buffer,int i) const;
        void end(const VkCommandBuffer& command_buffer) const;
        const VkRenderPass& getHandle() const;
        std::vector<VkFramebuffer>& getFrameBuffers();
    private:
        void createFramebuffers();


    };
}

