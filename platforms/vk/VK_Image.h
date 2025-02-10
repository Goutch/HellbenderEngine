#pragma once

#include <queue>
#include "core/resource/Image.h"
#include "VK_Device.h"
#include "vulkan/vulkan.h"
#include "VK_Allocator.h"
#include "array"

namespace HBE {
	class VK_Buffer;

	class VK_Fence;

	class VK_Renderer;

	class VK_Semaphore;

	class VK_Image : public Image {
		VkImage handle = VK_NULL_HANDLE;
		VK_Device *device;
		VkImageLayout layout;
		VkImageLayout desired_layout;

		std::vector<VkImageView> image_views;
		VkSampler sampler_handle = VK_NULL_HANDLE;

		ImageInfo info;

		VkFormat vk_format;
		Allocation allocation;
		uint32_t width = 1, height = 1, depth = 1;
		uint32_t byte_per_pixel;
		uint32_t id = 0;
		uint32_t mip_levels;
		static uint32_t current_id;
	public:
		VK_Image(VK_Device *device, const ImageInfo &info);
		~VK_Image();
		uint32_t getWidth() const override;
		uint32_t getHeight() const override;
		uint32_t getDepth() const override;
		vec3u getSize() const override;
		void update(const void *data,IMAGE_FORMAT format) override;
		void updateRegion(const void *data, uint32_t data_texel_count, ImageRegionUpdateInfo* update_info, uint32_t update_count) override;

		const VkSampler& getSampler() const;
		const VkImage& getHandle() const;
		const VkImageView& getImageView(uint32_t mip_level = 0) const;
		uint32_t getMipLevelCount() const;
		VkFormat getVkFormat() const;
		VkImageLayout getImageLayout() const;
		VkImageLayout getDesiredLayout() const;
		uint32_t bytePerPixel() const;
		void setImageLayout(VkImageLayout layout);

	private:
		VkImageLayout chooseLayout();
	};
}



