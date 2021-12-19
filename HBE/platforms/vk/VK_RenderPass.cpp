
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
			case IMAGE_FORMAT_DEPTH32F:
				return VK_FORMAT_D32_SFLOAT;
				break;
			case IMAGE_FORMAT_DEPTH32f_STENCIL8U:
				return VK_FORMAT_D32_SFLOAT_S8_UINT;
				break;
			case IMAGE_FORMAT_DEPTH24f_STENCIL8U:
				return VK_FORMAT_D24_UNORM_S8_UINT;
				break;
		}
		return VK_FORMAT_R32_SFLOAT;
	}

	VK_RenderPass::VK_RenderPass(VK_Renderer *renderer, const RenderTargetInfo &info) {
		this->renderer = renderer;
		this->device = renderer->getDevice();
		this->width = info.width;
		this->height = info.height;
		this->clear_color = info.clear_color;
		this->vk_format = getVkFormat(info.format);
		this->format = info.format;
		this->flags = info.flags;
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
				if (flags & RENDER_TARGET_FLAG_DEPTH_TEST)
					delete depth_images[i];
			}
			images.clear();
			if (flags & RENDER_TARGET_FLAG_DEPTH_TEST)
				depth_images.clear();
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

		VkAttachmentDescription depthAttachment{};
		VkAttachmentReference depthAttachmentRef{};

		if (flags & RENDER_TARGET_FLAG_DEPTH_TEST) {
			depthAttachment.format = getVkFormat(IMAGE_FORMAT_DEPTH32F);
			depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}


		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;
		if (flags & RENDER_TARGET_FLAG_DEPTH_TEST)
			subpass.pDepthStencilAttachment = &depthAttachmentRef;

		std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

		VkRenderPassCreateInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = (flags & RENDER_TARGET_FLAG_DEPTH_TEST) ? 2 : 1;
		render_pass_info.pAttachments = attachments.data();
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;

		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcStageMask |= (flags & RENDER_TARGET_FLAG_DEPTH_TEST) ? VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT : 0;
		dependency.srcAccessMask = 0;

		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask |= (flags & RENDER_TARGET_FLAG_DEPTH_TEST) ? VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT : 0;

		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask |= (flags & RENDER_TARGET_FLAG_DEPTH_TEST) ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : 0;

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

		for (size_t i = 0; i < images.size(); ++i) {
			delete images[i];
			if (flags & RENDER_TARGET_FLAG_DEPTH_TEST) delete depth_images[i];
		}
		for (auto framebuffer: frame_buffers) {
			vkDestroyFramebuffer(device->getHandle(), framebuffer, nullptr);
		}
		vkDestroyRenderPass(device->getHandle(), handle, nullptr);
	}

	void VK_RenderPass::begin(const VkCommandBuffer &command_buffer, uint32_t i) const {
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = handle;
		renderPassInfo.framebuffer = frame_buffers[i];

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = {width, height};

		if (flags & RENDER_TARGET_FLAG_DEPTH_TEST) {
			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = {clear_color.r, clear_color.g, clear_color.b, clear_color.a};
			clearValues[1].depthStencil = {1.0f, 0};
			renderPassInfo.clearValueCount = 2;
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		} else {
			VkClearValue clearColor = {clear_color.r, clear_color.g, clear_color.b, clear_color.a,};
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		}

	}

	const VkRenderPass &VK_RenderPass::getHandle() const {
		return handle;
	}

	void VK_RenderPass::end(const VkCommandBuffer &command_buffer) const {
		vkCmdEndRenderPass(command_buffer);
	}

	void VK_RenderPass::createFramebuffers() {

		std::vector<VkImageView> image_views;
		std::vector<VkImageView> depth_image_views;
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

		if ((flags & RENDER_TARGET_FLAG_DEPTH_TEST) == RENDER_TARGET_FLAG_DEPTH_TEST) {
			depth_images.resize(MAX_FRAMES_IN_FLIGHT);
			depth_image_views.resize(MAX_FRAMES_IN_FLIGHT);
			for (size_t i = 0; i < depth_images.size(); ++i) {
				TextureInfo info{};
				info.format = IMAGE_FORMAT_DEPTH32F;
				info.width = width;
				info.height = height;
				info.flags = IMAGE_FLAG_DEPTH;
				depth_images[i] = new VK_Image(renderer->getDevice(), info);
				depth_image_views[i] = depth_images[i]->getImageView();
			}
		}

		frame_buffers.resize(image_views.size());
		for (size_t i = 0; i < image_views.size(); ++i) {
			if (flags & RENDER_TARGET_FLAG_DEPTH_TEST) {
				std::array<VkImageView, 2> attachments = {
						image_views[i],
						depth_image_views[i]
				};

				VkFramebufferCreateInfo framebuffer_create_info{};
				framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebuffer_create_info.renderPass = handle;
				framebuffer_create_info.attachmentCount = attachments.size();
				framebuffer_create_info.pAttachments = attachments.data();
				framebuffer_create_info.width = width;
				framebuffer_create_info.height = height;
				framebuffer_create_info.layers = 1;
				if (vkCreateFramebuffer(device->getHandle(), &framebuffer_create_info, nullptr, &frame_buffers[i]) !=
					VK_SUCCESS) {
					Log::error("Failed to create framebuffer!");
				}

			} else {

				VkFramebufferCreateInfo framebuffer_create_info{};
				framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebuffer_create_info.renderPass = handle;
				framebuffer_create_info.attachmentCount = 1;
				framebuffer_create_info.pAttachments = &image_views[i];
				framebuffer_create_info.width = width;
				framebuffer_create_info.height = height;
				framebuffer_create_info.layers = 1;
				if (vkCreateFramebuffer(device->getHandle(), &framebuffer_create_info, nullptr, &frame_buffers[i]) !=
					VK_SUCCESS) {
					Log::error("Failed to create framebuffer!");
				}
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



