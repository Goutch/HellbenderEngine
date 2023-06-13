#pragma once

#include <queue>
#include "core/resource/Texture.h"
#include "VK_Device.h"
#include "vulkan/vulkan.h"
#include "VK_Allocator.h"
#include "array"

namespace HBE {
	class VK_Buffer;

	class VK_Fence;

	class VK_Renderer;

	class VK_Semaphore;

	class VK_Image : public Texture {
		VkImageLayout desired_layout;
		VK_Device *device;
		VkImage handle = VK_NULL_HANDLE;
		std::vector<VkImageView> image_views;
		VkSampler sampler_handle = VK_NULL_HANDLE;
		VkImageLayout layout;
		IMAGE_FORMAT format;
		VkFormat vk_format;
		Allocation allocation;
		uint32_t width = 1, height = 1, depth = 1;
		uint32_t byte_per_pixel;
		IMAGE_FLAGS flags;
		uint32_t id = 0;
		uint32_t mip_levels;
		static uint32_t current_id;
	public:
		VK_Image(VK_Device *device, const TextureInfo &info);
		~VK_Image();
		uint32_t getWidth() const override;
		uint32_t getHeight() const override;
		uint32_t getDepth() const override;
		vec3u getSize() const override;
		void update(const void *data) override;

		VkSampler getSampler() const;
		VkImage getHandle() const;
		void setImageLayout(VkImageLayout layout);
		VkImageLayout getImageLayout() const;
		uint32_t bytePerPixel() const;
		VkImageLayout getDesiredLayout() const;
		VkImageView getImageView(uint32_t mip_level = 0) const;
		uint32_t getMipLevelCount() const;
		VkFormat getVkFormat() const;
	private:
		VkImageLayout chooseLayout();

	};
}



