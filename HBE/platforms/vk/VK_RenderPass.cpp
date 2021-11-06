
#include "VK_RenderPass.h"
#include "VK_Swapchain.h"
#include "VK_Renderer.h"
#include "VK_CommandPool.h"
#include "VK_Fence.h"
namespace HBE {

    VkFormat getVkFormat(IMAGE_FORMAT format) {
        switch (format) {

            case IMAGE_FORMAT_R8:
                return VK_FORMAT_R8_SRGB;
                break;
            case IMAGE_FORMAT_RG8:
                return VK_FORMAT_R8G8_SRGB;
                break;
            case IMAGE_FORMAT_RGB8:
                return VK_FORMAT_R8G8B8_SRGB;
                break;
            case IMAGE_FORMAT_RGBA8:
                return VK_FORMAT_R8G8B8A8_SRGB;
                break;
            case IMAGE_FORMAT_R32F:
                return VK_FORMAT_R32_SFLOAT;
                break;
            case IMAGE_FORMAT_RG32F:
                return VK_FORMAT_R32G32_SFLOAT;
                break;
            case IMAGE_FORMAT_RGB32F:
                return VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case IMAGE_FORMAT_RGBA32F:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
                break;

        }
    }

    VK_RenderPass::VK_RenderPass(VK_Renderer *renderer, const RenderTargetInfo &info) {
        this->renderer = renderer;
        this->device = renderer->getDevice();
        this->width = info.width;
        this->height = info.height;
        this->clear_color = info.clear_color;
        this->vk_format = getVkFormat(info.format);
        this->format = info.format;
        recreate();
    }

    void VK_RenderPass::recreate() {
        if (handle != VK_NULL_HANDLE) {

			vkDestroyRenderPass(device->getHandle(), handle, nullptr);
            for (auto framebuffer: frame_buffers) {
                vkDestroyFramebuffer(device->getHandle(), framebuffer, nullptr);
            }
			for (size_t i = 0; i < images.size(); ++i) {
				delete images[i];
			}
			images.clear();
            frame_buffers.clear();

        }

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = vk_format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

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

        if (vkCreateRenderPass(device->getHandle(), &render_pass_info, nullptr, &handle) != VK_SUCCESS) {
            Log::error("failed to create render pass!");
        }
        createFramebuffers();
    }

    void VK_RenderPass::setClearColor(vec4 color) {
        this->clear_color = color;
    }

    const vec4 &VK_RenderPass::getClearColor() const {
        return clear_color;
    }

    VK_RenderPass::~VK_RenderPass() {

        for (int i = 0; i < images.size(); ++i) {
            delete images[i];
        }
        for (auto framebuffer: frame_buffers) {
            vkDestroyFramebuffer(device->getHandle(), framebuffer, nullptr);
        }
        vkDestroyRenderPass(device->getHandle(), handle, nullptr);
    }

    void VK_RenderPass::begin(const VkCommandBuffer& command_buffer,uint32_t i) const {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = handle;
        renderPassInfo.framebuffer = frame_buffers[i];

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {width, height};

        VkClearValue clearColor = {clear_color.r, clear_color.g, clear_color.b, clear_color.a};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    const VkRenderPass &VK_RenderPass::getHandle() const {
        return handle;
    }

    void VK_RenderPass::end(const VkCommandBuffer& command_buffer) const {
        vkCmdEndRenderPass(command_buffer);
    }

    void VK_RenderPass::createFramebuffers() {

        std::vector<VkImageView> image_views;
        images.resize(MAX_FRAMES_IN_FLIGHT);
        image_views.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < images.size(); ++i) {
            TextureInfo info{};
            info.format = format;
            info.width = width;
            info.height = height;
            info.flags = IMAGE_FLAG_RENDER_TARGET;
            images[i] = new VK_Image(renderer->getDevice(), info);
            image_views[i] = images[i]->getImageView();
        }

        frame_buffers.resize(image_views.size());
        for (size_t i = 0; i < image_views.size(); ++i) {
            VkImageView attachments[] = {
                    image_views[i]
            };
            VkFramebufferCreateInfo framebuffer_create_info{};
            framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_create_info.renderPass = handle;
            framebuffer_create_info.attachmentCount = 1;
            framebuffer_create_info.pAttachments = attachments;
            framebuffer_create_info.width = width;
            framebuffer_create_info.height = height;
            framebuffer_create_info.layers = 1;


            if (vkCreateFramebuffer(device->getHandle(), &framebuffer_create_info, nullptr, &frame_buffers[i]) !=
                VK_SUCCESS) {
                Log::error("Failed to create framebuffer!");
            }
        }
    }

    const std::vector<VkFramebuffer> &VK_RenderPass::getFrameBuffers() const {
        return frame_buffers;
    }

    void VK_RenderPass::setResolution(uint32_t width, uint32_t height) {
        this->width = width;
        this->height = height;
        recreate();
        onResolutionChange.invoke(this);
    }

    void VK_RenderPass::getResolution(uint32_t &width, uint32_t &height) const {
        width = this->width;
        height = this->height;
    }

    const VK_Image *VK_RenderPass::getImage(uint32_t i) const {
        return images[i];
    }




}



