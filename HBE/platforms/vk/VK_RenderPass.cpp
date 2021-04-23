//
// Created by user on 4/22/2021.
//

#include "VK_RenderPass.h"
#include "vulkan/vulkan.h"

HBE::VK_RenderPass::VK_RenderPass() {
    //todo: find out if renderpasss== framebuffer
    VkAttachmentDescription colorAttachment{};
    //todo: fix this
    //colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
}
