

#include "VK_Texture.h"
#include "VK_Buffer.h"
#include "VK_Allocator.h"
#include "core/utility/Log.h"
namespace HBE {

	void VK_Texture::setData(unsigned char *data, int width, int height, TEXTURE_FORMAT format) {
		if (handle != VK_NULL_HANDLE) {
			vkDestroyImage(device->getHandle(), handle, nullptr);
			device->getAllocator().free(*allocation);
		}
		VkFormat vk_format;
		switch (format) {
			case R8:
				vk_format = VK_FORMAT_R8_SRGB;
				break;
			case RG8:
				vk_format = VK_FORMAT_R8G8_SRGB;
				break;
			case RGB8:
				vk_format = VK_FORMAT_R8G8B8_SRGB;
				break;
			case RGBA8:
				vk_format = VK_FORMAT_R8G8B8A8_SRGB;
				break;
			case R32F:
				vk_format = VK_FORMAT_R32_SFLOAT;
				break;
			case RG32F:
				vk_format = VK_FORMAT_R32G32_SFLOAT;
				break;
			case RGB32F:
				vk_format = VK_FORMAT_R32G32B32_SFLOAT;
				break;
			case RGBA32F:
				vk_format = VK_FORMAT_R32G32B32A32_SFLOAT;
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

		VK_Buffer staging_buffer = VK_Buffer(device, data, requirements.size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, MAPPABLE);

		device->getAllocator().copy(staging_buffer.getHandle(), handle, width, height);

	}

	void VK_Texture::bind(unsigned int slot) const {

	}

	void VK_Texture::unbind(unsigned int slot) const {

	}

	VK_Texture::VK_Texture(VK_Device *device) {
		this->device = device;

	}

	VK_Texture::~VK_Texture() {
		vkDestroyImage(device->getHandle(), handle, nullptr);
		device->getAllocator().free(*allocation);
	}


}

