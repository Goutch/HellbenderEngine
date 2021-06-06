//
// Created by user on 4/22/2021.
//

#include "VK_RenderPass.h"
#include "vulkan/vulkan.h"
#include "VK_Device.h"
#include "core/utility/Log.h"
#include "VK_Swapchain.h"
namespace HBE{

    VK_RenderPass::VK_RenderPass(const VK_Device *device, const VK_Swapchain *swapchain) {
        this->device=device;
        this->swapchain=swapchain;
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapchain->getFormat();
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

        if (vkCreateRenderPass(device->getHandle(), &render_pass_info, nullptr, &handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
        createFramebuffers();
    }
    VK_RenderPass::~VK_RenderPass() {
        for (auto framebuffer : frame_buffers) {
            vkDestroyFramebuffer(device->getHandle(), framebuffer, nullptr);
        }
        vkDestroyRenderPass(device->getHandle(), handle, nullptr);
    }
    void VK_RenderPass::begin(VkCommandBuffer const &command_buffer, int i) const {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = handle;
        renderPassInfo.framebuffer = frame_buffers[i];

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain->getExtent();

        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    const VkRenderPass& VK_RenderPass::getHandle() const{
        return handle;
    }

    void VK_RenderPass::end(const VkCommandBuffer& command_buffer) const {
        vkCmdEndRenderPass(command_buffer);
    }

    void VK_RenderPass::createFramebuffers() {
        auto image_views=swapchain->getImagesViews();
        extent=swapchain->getExtent();
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
            framebuffer_create_info.width = extent.width;
            framebuffer_create_info.height = extent.height;
            framebuffer_create_info.layers = 1;


            if (vkCreateFramebuffer(device->getHandle(), &framebuffer_create_info, nullptr, &frame_buffers[i]) !=
                VK_SUCCESS) {
                Log::error("Failed to create framebuffer!");
            }
        }
    }

    std::vector<VkFramebuffer> &VK_RenderPass::getFrameBuffers() {
        return frame_buffers;
    }




}



