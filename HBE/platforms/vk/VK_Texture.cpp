

#include "VK_Texture.h"
#include "VK_Buffer.h"
#include "VK_Renderer.h"
#include "VK_CommandPool.h"

namespace HBE {

	void VK_Texture::setData(unsigned char *data, int width, int height, TEXTURE_FORMAT format) {
		if (view_hanlde != VK_NULL_HANDLE) {
			vkDestroyImageView(device->getHandle(), view_hanlde, nullptr);
		}
		if (handle != VK_NULL_HANDLE) {
			vkDestroyImage(device->getHandle(), handle, nullptr);
			device->getAllocator().free(*allocation);
		}
		VkDeviceSize byte_per_pixel;
		VkFormat vk_format;
		switch (format) {
			case R8:
				vk_format = VK_FORMAT_R8_SRGB;
				byte_per_pixel=1;
				break;
			case RG8:
				vk_format = VK_FORMAT_R8G8_SRGB;
				byte_per_pixel=2;
				break;
			case RGB8:
				vk_format = VK_FORMAT_R8G8B8_SRGB;
				byte_per_pixel=3;
				break;
			case RGBA8:
				vk_format = VK_FORMAT_R8G8B8A8_SRGB;
				byte_per_pixel=4;
				break;
			case R32F:
				vk_format = VK_FORMAT_R32_SFLOAT;
				byte_per_pixel=4;
				break;
			case RG32F:
				vk_format = VK_FORMAT_R32G32_SFLOAT;
				byte_per_pixel=4*2;
				break;
			case RGB32F:
				vk_format = VK_FORMAT_R32G32B32_SFLOAT;
				byte_per_pixel=4*3;
				break;
			case RGBA32F:
				vk_format = VK_FORMAT_R32G32B32A32_SFLOAT;
				byte_per_pixel=4*4;
				break;

		}

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;

		imageInfo.format = vk_format;

		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0; // Optional

		if (vkCreateImage(device->getHandle(), &imageInfo, nullptr, &handle) != VK_SUCCESS) {
			Log::error("failed to create image!");
		}

		VkMemoryRequirements requirements;
		vkGetImageMemoryRequirements(device->getHandle(), handle, &requirements);

		this->allocation = &device->getAllocator().alloc(requirements, NONE);

		vkBindImageMemory(device->getHandle(), handle, allocation->block.memory, allocation->offset);

		VK_Buffer staging_buffer = VK_Buffer(device, data, width*height*byte_per_pixel, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, MAPPABLE);

		device->getAllocator().copy(staging_buffer.getHandle(), handle, width, height);

		//image view
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = handle;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device->getHandle(), &viewInfo, nullptr, &view_hanlde) != VK_SUCCESS) {
			Log::error("failed to create texture image view!");
		}
	}

	VK_Texture::VK_Texture(VK_Device *device) {
		this->device = device;
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

		samplerInfo.anisotropyEnable = device->getPhysicalDevice().getFeatures().samplerAnisotropy;
		samplerInfo.maxAnisotropy = device->getPhysicalDevice().getProperties().limits.maxSamplerAnisotropy;

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;


		if (vkCreateSampler(device->getHandle(), &samplerInfo, nullptr, &sampler_handle) != VK_SUCCESS) {
			Log::error("failed to create texture sampler!");
		}
	}

	VK_Texture::~VK_Texture() {
		if (sampler_handle != VK_NULL_HANDLE) {
			vkDestroySampler(device->getHandle(), sampler_handle, nullptr);
		}
		if (view_hanlde != VK_NULL_HANDLE) {
			vkDestroyImageView(device->getHandle(), view_hanlde, nullptr);
		}
		if (handle != VK_NULL_HANDLE) {
			vkDestroyImage(device->getHandle(), handle, nullptr);
			device->getAllocator().free(*allocation);
		}
	}

	const VkSampler &VK_Texture::getSampler() const {
		return sampler_handle;
	}

	const VkImageView &VK_Texture::getImageView() const {
		return view_hanlde;
	}

	const VkImage &VK_Texture::getImage() const {
		return handle;
	}


}

