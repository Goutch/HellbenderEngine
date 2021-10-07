#pragma once

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>
#include "vector"


namespace HBE {
    class VK_PhysicalDevice;

    class VK_RenderPass;

    class VK_Device;

    class VK_Swapchain {
        VkSwapchainKHR handle{};
		const VkSurfaceKHR *surface_handle= nullptr;
        const VK_PhysicalDevice *physical_device = nullptr;
        const VK_Device *device = nullptr;
        uint32_t image_count;
        uint32_t width;
        uint32_t height;
        std::vector<VkImage> images;
        std::vector<VkImageView> image_views;
        VkExtent2D extent;
        VkFormat format;
        VK_RenderPass* render_pass= nullptr;
    public:
        VK_Swapchain(uint32_t width,
                     uint32_t height,
                     const VkSurfaceKHR &surface_handle,
                     const VK_Device &device);

        ~VK_Swapchain();
		void recreate();
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        void createImageViews();

        VkSwapchainKHR &getHandle();

        const VkExtent2D &getExtent() const;

        const VkFormat &getFormat() const;

        const std::vector<VkImageView> &getImagesViews() const;

        const VK_RenderPass& getRenderPass() const;
    };
}
