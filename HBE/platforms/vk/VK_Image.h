#pragma once

#include "core/resource/Texture.h"
#include "VK_Device.h"
#include "vulkan/vulkan.h"
#include "VK_Allocator.h"


namespace HBE {
	class VK_Buffer;

	class VK_Renderer;

	class VK_Image : public Texture {
		VK_Device *device;
		VkImage handle = VK_NULL_HANDLE;
		VkImageView view_hanlde = VK_NULL_HANDLE;
		VkSampler sampler_handle = VK_NULL_HANDLE;
		VkImageLayout layout;
		IMAGE_FORMAT format;
		VkFormat vk_format;
		Allocation *allocation = nullptr;
		uint32_t width = 1, height = 1, depth = 1;
		uint32_t byte_per_pixel;
		IMAGE_FLAGS flags;
	public:
		VK_Image(VK_Device *device, const TextureInfo &info);
		~VK_Image();
		uint32_t getWidth() const override;
		uint32_t getHeight() const override;
		uint32_t getDepth() const override;
		vec3u getSize() const override;
		void update(const void *data) override;

		const VkSampler &getSampler() const;
		const VkImageView &getImageView() const;
		const VkImage &getHandle() const;
		void setImageLayout(VkImageLayout layout);
		const VkImageLayout getImageLayout() const;
		const VkFormat getVkFormat() const;

		VkImageLayout chooseLayout();
	};
}



