//
// Created by user on 4/22/2021.
//

#include "VK_RenderPass.h"
#include "vulkan/vulkan.h"

HBE::VK_RenderPass::VK_RenderPass(uint32_t width,uint32_t height) {



    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
}
