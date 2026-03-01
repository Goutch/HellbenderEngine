#include "VK_Swapchain.h"
#include "core/utility/Log.h"
#include <algorithm>
#include <cstdint>
#include "VK_Swapchain.h"
#include "VK_Device.h"
#include "VK_PhysicalDevice.h"
#include "VK_RenderPass.h"
#include "VK_Surface.h"
#include "core/Application.h"
#include "core/Configs.h"

namespace HBE
{
    void VK_Swapchain::init(VK_Device& device, VK_Surface& surface)
    {
        this->device = &device;
        this->physical_device = &device.getPhysicalDevice();
        this->surface = &surface;
        ApplicationInfo info = Application::instance->getInfo();
        recreate(info.window_info.startSize.x, info.window_info.startSize.y);
    }

    void VK_Swapchain::release()
    {
        if (render_pass != VK_NULL_HANDLE)
        {
            for (auto framebuffer : frame_buffers)
            {
                vkDestroyFramebuffer(device->getHandle(), framebuffer, nullptr);
            }
            vkDestroyRenderPass(device->getHandle(), render_pass, nullptr);
        }
        for (auto imageView : image_views)
        {
            vkDestroyImageView(device->getHandle(), imageView, nullptr);
        }
        vkDestroySwapchainKHR(device->getHandle(), handle, nullptr);
    }

    void VK_Swapchain::recreate(uint32_t width, uint32_t height)
    {
        this->extent.width = width;
        this->extent.height = height;
        if (handle != VK_NULL_HANDLE)
        {
            for (auto imageView : image_views)
            {
                vkDestroyImageView(device->getHandle(), imageView, nullptr);
            }
            vkDestroySwapchainKHR(device->getHandle(), handle, nullptr);
        }

        SwapchainSupportDetails details = physical_device->querySwapchainSupportDetails(physical_device->getHandle());
        image_count = details.capabilities.minImageCount + 1;
        if (details.capabilities.maxImageCount > 0 && image_count > details.capabilities.maxImageCount)
            image_count = details.capabilities.maxImageCount;
        VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(details.formats);
        VkPresentModeKHR present_mode = chooseSwapPresentMode(details.present_modes);
        chooseSwapExtent(details.capabilities);
        format = surface_format.format;


        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = surface->getHandle();
        create_info.imageExtent = extent;
        create_info.minImageCount = image_count;
        create_info.imageFormat = format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


        QueueFamilyIndices indices = physical_device->getQueueFamilyIndices();

        uint32_t queue_family_indices[] = {
            indices.graphics_family.value(),
            indices.present_family.value()
        };
        if (indices.graphics_family != indices.present_family)
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queue_family_indices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0;
            create_info.pQueueFamilyIndices = nullptr;
        }
        create_info.preTransform = details.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device->getHandle(), &create_info, nullptr, &handle) != VK_SUCCESS)
        {
            Log::error("Failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device->getHandle(), handle, &image_count, nullptr);
        images.resize(image_count);
        vkGetSwapchainImagesKHR(device->getHandle(), handle, &image_count, images.data());

        createImageViews();

        Log::status(std::string("Created swapchain with extent:") + std::to_string(extent.width) + "x" +
            std::to_string(extent.height));

        createRenderPass();
    }

    VkSurfaceFormatKHR VK_Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
    {
        for (const auto& available_format : available_formats)
        {
            if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                available_format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
            {
                return available_format;
            }
        }
        return available_formats[0];
    }

    VkPresentModeKHR VK_Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
    {
        VkPresentModeKHR preferred = Configs::getVerticalSync() ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
        for (const auto& available_present_mode : available_present_modes)
        {
            if (available_present_mode == preferred)
                return preferred;
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    void VK_Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            extent = capabilities.currentExtent;
        }
        else
        {
            extent.width = std::clamp(extent.width, capabilities.minImageExtent.width,
                                      capabilities.maxImageExtent.width);
            extent.height = std::clamp(extent.height, capabilities.minImageExtent.height,
                                       capabilities.maxImageExtent.height);
        }
    }

    void VK_Swapchain::createImageViews()
    {
        image_views.resize(images.size());
        for (size_t i = 0; i < image_views.size(); ++i)
        {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = images[i];
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = format;
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;
            if (vkCreateImageView(device->getHandle(), &create_info, nullptr, &image_views[i]) !=
                VK_SUCCESS)
            {
                Log::error("Failed to create image views");
            }
        }
    }

    const VkExtent2D& VK_Swapchain::getExtent() const
    {
        return extent;
    }

    VkSwapchainKHR& VK_Swapchain::getHandle()
    {
        return handle;
    }

    const VkFormat& VK_Swapchain::getFormat() const
    {
        return format;
    }

    const std::vector<VkImageView>& VK_Swapchain::getImagesViews() const
    {
        return image_views;
    }

    void VK_Swapchain::createFramebuffers()
    {
        frame_buffers.resize(image_views.size());
        for (size_t i = 0; i < image_views.size(); ++i)
        {
            VkImageView attachments[] = {
                image_views[i]
            };
            VkFramebufferCreateInfo framebuffer_create_info{};
            framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_create_info.renderPass = render_pass;
            framebuffer_create_info.attachmentCount = 1;
            framebuffer_create_info.pAttachments = attachments;
            framebuffer_create_info.width = extent.width;
            framebuffer_create_info.height = extent.height;
            framebuffer_create_info.layers = 1;


            if (vkCreateFramebuffer(device->getHandle(), &framebuffer_create_info, nullptr, &frame_buffers[i]) !=
                VK_SUCCESS)
            {
                Log::error("Failed to create framebuffer!");
            }
        }
    }

    void VK_Swapchain::createRenderPass()
    {
        if (render_pass != VK_NULL_HANDLE)
        {
            for (auto framebuffer : frame_buffers)
            {
                vkDestroyFramebuffer(device->getHandle(), framebuffer, nullptr);
            }
            vkDestroyRenderPass(device->getHandle(), render_pass, nullptr);
        }

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref{};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments = &colorAttachment;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;

        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;

        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        render_pass_info.dependencyCount = 1;
        render_pass_info.pDependencies = &dependency;

        if (vkCreateRenderPass(device->getHandle(), &render_pass_info, nullptr, &render_pass) != VK_SUCCESS)
        {
            Log::error("failed to create rasterize pass!");
        }
        createFramebuffers();
    }

    void VK_Swapchain::beginRenderPass(uint32_t i, const VkCommandBuffer& command_buffer)
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = render_pass;
        renderPassInfo.framebuffer = frame_buffers[i];

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;

        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VK_Swapchain::endRenderPass(const VkCommandBuffer& command_buffer)
    {
        vkCmdEndRenderPass(command_buffer);
    }

    uint32_t VK_Swapchain::getImagesCount() const
    {
        return image_views.size();
    }

    VkRenderPass& VK_Swapchain::getRenderPass()
    {
        return render_pass;
    }
}
