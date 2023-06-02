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

		const VkSampler &getSampler() const;
		const VkImage &getHandle() const;
		void setImageLayout(VkImageLayout layout);
		const VkImageLayout getImageLayout() const;
		static VkFormat getVkFormat(IMAGE_FORMAT format);
		const VkFormat getVkFormat() const;
		static uint32_t getBytePerPixel(IMAGE_FORMAT format);
		const uint32_t bytePerPixel() const;
		const VkImageLayout getDesiredLayout() const;
		VkImageView const &getImageView(uint32_t mip_level = 0) const;
		uint32_t getMipLevelCount() const;
	private:
		VkImageLayout chooseLayout();

	};
}



