#pragma once

#include <core/resource/RasterizationTarget.h>
#include "vulkan/vulkan.h"
#include "vector"
#include "VK_Image.h"


namespace HBE {
	class VK_Device;

	class VK_Swapchain;

	class VK_Semaphore;

	class VK_Fence;

	class VK_RenderPass : public RasterizationTarget {
		VkRenderPass handle = VK_NULL_HANDLE;
		uint32_t width = 0, height = 0;
		vec4 clear_color = vec4(0.f, 0.f, 0.f, 1.f);
		VK_Renderer *renderer;
		VK_Device *device;
		VkExtent2D extent;

		std::vector<VK_Image *> depth_images;
		std::vector<VK_Image *> images;
		std::vector<VkFramebuffer> frame_buffers;
		uint32_t current_frame = 0;
		VkFormat vk_format;
		IMAGE_FORMAT format;
		RENDER_TARGET_FLAGS flags;
		bool has_color_attachment = false;
		bool has_depth_attachment = false;
		bool clear_color_enabled = false;
		bool clear_depth_enabled = false;
	public:
		~VK_RenderPass();

		VK_RenderPass(VK_Renderer *renderer, const RenderTargetInfo &info);

		void begin(const VkCommandBuffer &command_buffer, uint32_t i) const;

		void end(const VkCommandBuffer &command_buffer) const;

		void setClearColor(vec4 color) override;

		const vec4 &getClearColor() const override;

		const VkRenderPass &getHandle() const;
		Image *getFramebufferTexture(uint32_t frame) const override;

		const std::vector<VkFramebuffer> &getFrameBuffers() const;

		void setResolution(uint32_t width, uint32_t height) override;

		vec2i getResolution() const override;

		const VK_Image *getImage(uint32_t i) const;

		void recreate();

	protected:
		virtual void createFramebuffers();
	};
}

