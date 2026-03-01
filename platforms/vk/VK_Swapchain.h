#pragma once

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>
#include "vector"


namespace HBE
{
    class VK_Context;
    class VK_Surface;
    class VK_PhysicalDevice;

    class VK_RenderPass;

    class VK_Device;

    class VK_Semaphore;

    class VK_Swapchain
    {
        VkSwapchainKHR handle{};

        VK_Context* context = nullptr;

        uint32_t image_count;
        std::vector<VkImage> images;
        std::vector<VkImageView> image_views;
        std::vector<VkFramebuffer> frame_buffers;
        VkRenderPass render_pass = VK_NULL_HANDLE;
        VkExtent2D extent;
        VkFormat format;

    public:
        void init(VK_Context* context);
        void release();

        VK_Swapchain() = default;
        ~VK_Swapchain() = default;
        void recreate(uint32_t width, uint32_t height);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);

        void chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        void createImageViews();

        VkSwapchainKHR& getHandle();

        const VkExtent2D& getExtent() const;

        const VkFormat& getFormat() const;

        const std::vector<VkImageView>& getImagesViews() const;

        void beginRenderPass(uint32_t i, const VkCommandBuffer& command_buffer);
        void endRenderPass(const VkCommandBuffer& command_buffer);
        uint32_t getImagesCount() const;
        VkRenderPass& getRenderPass();

    private:
        void createRenderPass();
        void createFramebuffers();
    };
}
