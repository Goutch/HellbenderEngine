
#include "VK_RenderPass.h"
#include "VK_Swapchain.h"
#include "VK_Renderer.h"
#include "VK_CommandPool.h"
#include "VK_Fence.h"
#include "core/utility/Log.h"
#include "VK_Utils.h"

namespace HBE {

	VK_RenderPass::VK_RenderPass(VK_Renderer *renderer, const RenderTargetInfo &info) {
		this->renderer = renderer;
		this->device = renderer->getDevice();
		this->width = info.width;
		this->height = info.height;
		this->clear_color = info.clear_color;
		this->vk_format = VK_Utils::getVkFormat(info.format);
		this->format = info.format;
		this->has_color_attachment = info.flags & RENDER_TARGET_FLAG_COLOR_ATTACHMENT;
		this->has_depth_attachment = info.flags & RENDER_TARGET_FLAG_DEPTH_ATTACHMENT;
		this->clear_color_enabled = info.flags & RENDER_TARGET_FLAG_CLEAR_COLOR;
		this->clear_depth_enabled = info.flags & RENDER_TARGET_FLAG_CLEAR_DEPTH;
		this->flags = info.flags;
		recreate();
	}

	void VK_RenderPass::recreate() {
		if (handle != VK_NULL_HANDLE) {
			renderer->waitAll();
			vkDestroyRenderPass(device->getHandle(), handle, nullptr);
			for (auto framebuffer: frame_buffers) {
				vkDestroyFramebuffer(device->getHandle(), framebuffer, nullptr);
			}
			for (size_t i = 0; i < images.size(); ++i) {
				if (has_color_attachment)
					delete images[i];
				if (has_depth_attachment)
					delete depth_images[i];
			}

			images.clear();
			depth_images.clear();
			frame_buffers.clear();
		}

		VkRenderPassCreateInfo render_pass_info{};
		std::vector<VkSubpassDescription> subpass_descriptions;
		std::vector<VkSubpassDependency> subpass_dependencies;
		std::vector<VkAttachmentDescription> attachments;
		VkAttachmentReference color_attachment_ref{};
		VkAttachmentReference depth_attachment_ref{};

		if (has_color_attachment) {

			VkAttachmentDescription colorAttachment{};
			colorAttachment.format = vk_format;
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = (flags & RENDER_TARGET_FLAG_CLEAR_COLOR) ? VK_ATTACHMENT_LOAD_OP_CLEAR
			                                                                  : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = (flags & RENDER_TARGET_FLAG_CLEAR_COLOR) ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachments.push_back(colorAttachment);

			color_attachment_ref.attachment = attachments.size() - 1;
			color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		if (has_depth_attachment) {
			VkAttachmentDescription depthAttachment{};
			depthAttachment.format = VK_Utils::getVkFormat(IMAGE_FORMAT_DEPTH32F);
			depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depthAttachment.loadOp = (flags & RENDER_TARGET_FLAG_CLEAR_DEPTH) ? VK_ATTACHMENT_LOAD_OP_CLEAR
			                                                                  : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			if (!(flags & RENDER_TARGET_FLAG_CLEAR_DEPTH))
				Log::warning("Not clearing depth buffer will result in all depth test failing if the depth is not initialized to 1.0");
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.stencilLoadOp = (flags & RENDER_TARGET_FLAG_CLEAR_DEPTH) ? VK_ATTACHMENT_LOAD_OP_CLEAR
			                                                                         : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachments.push_back(depthAttachment);

			depth_attachment_ref.attachment = attachments.size() - 1;
			depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.pColorAttachments = has_color_attachment ? &color_attachment_ref : nullptr;
		subpass.pDepthStencilAttachment = has_depth_attachment ? &depth_attachment_ref : nullptr;
		subpass.colorAttachmentCount = 1;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_descriptions.push_back(subpass);


		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = attachments.size();
		render_pass_info.pAttachments = attachments.data();
		render_pass_info.subpassCount = subpass_descriptions.size();
		render_pass_info.pSubpasses = subpass_descriptions.data();
		render_pass_info.dependencyCount = subpass_dependencies.size();
		render_pass_info.pDependencies = subpass_dependencies.data();


		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;

		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcStageMask |= (flags & RENDER_TARGET_FLAG_DEPTH_ATTACHMENT) ? VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT : 0;
		dependency.srcAccessMask = 0;

		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask |= (flags & RENDER_TARGET_FLAG_DEPTH_ATTACHMENT) ? VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT : 0;

		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask |= (flags & RENDER_TARGET_FLAG_DEPTH_ATTACHMENT) ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : 0;

		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;

		if (vkCreateRenderPass(device->getHandle(), &render_pass_info,
		                       nullptr, &handle) != VK_SUCCESS) {
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
			if (has_color_attachment)
				delete images[i];
			if (has_depth_attachment)
				delete depth_images[i];
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


		VkClearValue clear_values[2] = {VkClearValue{}, VkClearValue{}};
		uint32_t clear_value_count = 0;
		if (has_color_attachment) {
			clear_values[clear_value_count].color = {clear_color.x, clear_color.y, clear_color.z, clear_color.w};
			clear_value_count++;
		}
		if (has_depth_attachment) {
			clear_values[clear_value_count].depthStencil = {1.0f, 0};
			clear_value_count++;
		}

		renderPassInfo.pClearValues = clear_values;
		renderPassInfo.clearValueCount = clear_value_count;
		vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	const VkRenderPass &VK_RenderPass::getHandle() const {
		return handle;
	}

	void VK_RenderPass::end(const VkCommandBuffer &command_buffer) const {
		vkCmdEndRenderPass(command_buffer);

		/*VkMemoryBarrier2 image_barrier{};
		image_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
		image_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		image_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		image_barrier.srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT;
		image_barrier.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		VkDependencyInfo dependency_info{};
		dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
		dependency_info.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		dependency_info.pImageMemoryBarriers = nullptr;
		dependency_info.imageMemoryBarrierCount = 0;
		dependency_info.pBufferMemoryBarriers = nullptr;
		dependency_info.bufferMemoryBarrierCount = 0;
		dependency_info.pMemoryBarriers = &image_barrier;
		dependency_info.memoryBarrierCount = 1;
		device->vkCmdPipelineBarrier2KHR(command_buffer, &dependency_info);*/
	}

	void VK_RenderPass::createFramebuffers() {

		std::vector<VkImageView> image_views;
		std::vector<VkImageView> depth_image_views;
		images.resize(MAX_FRAMES_IN_FLIGHT);
		image_views.resize(MAX_FRAMES_IN_FLIGHT);

		if (has_color_attachment) {
			for (size_t i = 0; i < images.size(); ++i) {
				TextureInfo info{};
				info.format = format;
				info.width = width;
				info.height = height;
				info.generate_mip_maps = false;
				info.flags = IMAGE_FLAG_RENDER_TARGET;
				images[i] = new VK_Image(renderer->getDevice(), info);
				image_views[i] = images[i]->getImageView();
			}
		}


		if (has_depth_attachment) {
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

		frame_buffers.resize(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			std::vector<VkImageView> attachments;

			if (has_color_attachment) {
				attachments.push_back(image_views[i]);
			}
			if (has_depth_attachment) {
				attachments.push_back(depth_image_views[i]);
			}

			VkFramebufferCreateInfo framebuffer_create_info{};
			framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_create_info.renderPass = handle;
			framebuffer_create_info.attachmentCount = has_color_attachment + has_depth_attachment;
			framebuffer_create_info.pAttachments = attachments.data();
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
		this->width = width > 0 ? width : 1;
		this->height = height > 0 ? height : 1;

		recreate();
		onResolutionChange.invoke(this);
	}

	vec2i VK_RenderPass::getResolution() const {
		return vec2i(width, height);
	}

	const VK_Image *VK_RenderPass::getImage(uint32_t i) const {
		return images[i];
	}

	Texture *VK_RenderPass::getFramebufferTexture(uint32_t frame) const {
		HB_ASSERT(frame < images.size(), "Frame index out of bounds");
		return images[frame];
	}

}



