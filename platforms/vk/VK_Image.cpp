

#include "VK_Image.h"
#include "VK_Buffer.h"
#include "VK_Renderer.h"
#include "VK_CommandPool.h"
#include "VK_Fence.h"
#include "VK_Semaphore.h"

namespace HBE {
	uint32_t VK_Image::current_id = 0;

	VkImageLayout VK_Image::chooseLayout() {
		if (flags & IMAGE_FLAG_DEPTH) {
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		} else if (flags & IMAGE_FLAG_RENDER_TARGET && !(flags & IMAGE_FLAG_SHADER_WRITE)) {
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		return VK_IMAGE_LAYOUT_GENERAL;
	}

	void VK_Image::update(const void *data) {
		device->getAllocator()->update(*this, data, width, height, depth);
	}

	uint32_t VK_Image::getBytePerPixel(IMAGE_FORMAT format) {
		uint32_t byte_per_pixel = 0;
		switch (format) {
			case IMAGE_FORMAT_R8:
			case IMAGE_FORMAT_SR8_NON_LINEAR:
				return 1;
				break;
			case IMAGE_FORMAT_RG8:
			case IMAGE_FORMAT_SRG8_NON_LINEAR:

				return 2;
				break;
			case IMAGE_FORMAT_RGB8:
			case IMAGE_FORMAT_SRGB8_NON_LINEAR:
				return 3;

			case IMAGE_FORMAT_RGBA8:
			case IMAGE_FORMAT_SRGBA8_NON_LINEAR:
			case IMAGE_FORMAT_SBGRA8_NON_LINEAR:
			case IMAGE_FORMAT_R32F:
				return 4;


			case IMAGE_FORMAT_RG32F:
				return 8;
				break;
			case IMAGE_FORMAT_RGB32F:
				return 12;
				break;
			case IMAGE_FORMAT_RGBA32F:
				return 16;
			case IMAGE_FORMAT_DEPTH32F:
				return 4;
				break;
			case IMAGE_FORMAT_DEPTH32f_STENCIL8U:
				return 5;
				break;
			case IMAGE_FORMAT_DEPTH24f_STENCIL8U:
				return 4;
				break;

		}
		return 0;
	}

	VkFormat VK_Image::getVkFormat(IMAGE_FORMAT format) {
		VkFormat vk_format = VK_FORMAT_UNDEFINED;
		switch (format) {
			case IMAGE_FORMAT_R8:
				vk_format = VK_FORMAT_R8_UINT;
				break;
			case IMAGE_FORMAT_RG8:
				vk_format = VK_FORMAT_R8G8_UINT;
				break;
			case IMAGE_FORMAT_RGB8:
				vk_format = VK_FORMAT_R8G8B8_UINT;
				break;
			case IMAGE_FORMAT_RGBA8:
				vk_format = VK_FORMAT_R8G8B8A8_UINT;
				break;
			case IMAGE_FORMAT_R32F:
				vk_format = VK_FORMAT_R32_SFLOAT;
				break;
			case IMAGE_FORMAT_RG32F:
				vk_format = VK_FORMAT_R32G32_SFLOAT;
				break;
			case IMAGE_FORMAT_RGB32F:
				vk_format = VK_FORMAT_R32G32B32_SFLOAT;
				break;
			case IMAGE_FORMAT_RGBA32F:
				vk_format = VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			case IMAGE_FORMAT_DEPTH32F:
				// look into choosing depth with device format properties: https://vulkan-tutorial.com/Depth_buffering
				vk_format = VK_FORMAT_D32_SFLOAT;
				break;
			case IMAGE_FORMAT_DEPTH32f_STENCIL8U:
				vk_format = VK_FORMAT_D32_SFLOAT_S8_UINT;
				break;
			case IMAGE_FORMAT_DEPTH24f_STENCIL8U:
				vk_format = VK_FORMAT_D24_UNORM_S8_UINT;
				break;
			case IMAGE_FORMAT_SBGRA8_NON_LINEAR:
				vk_format = VK_FORMAT_B8G8R8A8_SRGB;
				break;
			case IMAGE_FORMAT_SR8_NON_LINEAR:
				vk_format = VK_FORMAT_R8_SRGB;
				break;
			case IMAGE_FORMAT_SRG8_NON_LINEAR:
				vk_format = VK_FORMAT_R8G8_SRGB;
				break;
			case IMAGE_FORMAT_SRGB8_NON_LINEAR:
				vk_format = VK_FORMAT_R8G8B8_SRGB;
				break;
			case IMAGE_FORMAT_SRGBA8_NON_LINEAR:
				vk_format = VK_FORMAT_R8G8B8A8_SRGB;
				break;
		}
		return vk_format;
	}

	VK_Image::VK_Image(VK_Device *device, const TextureInfo &info) {
		this->device = device;
		this->width = info.width;
		this->height = info.height;
		this->depth = info.depth;
		this->format = info.format;
		this->flags = info.flags;
		this->id = current_id++;
		this->mip_levels = info.generate_mip_maps ? static_cast<uint32_t>(std::floor(std::log2(std::max(std::max(info.width, info.height), info.depth)))) + 1 : 1;

		desired_layout = chooseLayout();

		Log::debug("Create image#" + std::to_string(id));
		VkImageType type;
		VkImageViewType view_type;
		if (height == 1 && depth == 1) {
			type = VK_IMAGE_TYPE_1D;
			view_type = VK_IMAGE_VIEW_TYPE_1D;
		} else if (depth == 1) {
			type = VK_IMAGE_TYPE_2D;
			view_type = VK_IMAGE_VIEW_TYPE_2D;
		} else {
			type = VK_IMAGE_TYPE_3D;
			view_type = VK_IMAGE_VIEW_TYPE_3D;
		}
		vk_format = getVkFormat(format);
		byte_per_pixel = getBytePerPixel(format);

		layout = VK_IMAGE_LAYOUT_UNDEFINED;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = type;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = static_cast<uint32_t>(depth);
		imageInfo.mipLevels = mip_levels;
		imageInfo.arrayLayers = 1;

		imageInfo.format = vk_format;

		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		std::vector<uint32_t> queues = {device->getQueue(QUEUE_FAMILY_GRAPHICS).getFamilyIndex()};
		if (device->hasQueue(QUEUE_FAMILY_TRANSFER))device->getQueue(QUEUE_FAMILY_TRANSFER).getFamilyIndex();
		if (device->hasQueue(QUEUE_FAMILY_COMPUTE)) device->getQueue(QUEUE_FAMILY_COMPUTE).getFamilyIndex();

		imageInfo.sharingMode = queues.size()>1?VK_SHARING_MODE_CONCURRENT:VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.pQueueFamilyIndices = queues.data();
		imageInfo.queueFamilyIndexCount = queues.size();
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0; // Optional
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		imageInfo.usage |= (info.flags & IMAGE_FLAG_NO_SAMPLER) ? 0 : VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.usage |= (info.flags & IMAGE_FLAG_RENDER_TARGET) ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0;
		imageInfo.usage |= (info.flags & IMAGE_FLAG_DEPTH) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : 0;
		imageInfo.usage |= (info.flags & IMAGE_FLAG_SHADER_WRITE || info.flags & IMAGE_FLAG_NO_SAMPLER) ? VK_IMAGE_USAGE_STORAGE_BIT : 0;
		imageInfo.usage |= (info.generate_mip_maps ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0);
		if (vkCreateImage(device->getHandle(), &imageInfo, nullptr, &handle) != VK_SUCCESS) {
			Log::error("failed to create image!");
		}

		VkMemoryRequirements requirements;
		vkGetImageMemoryRequirements(device->getHandle(), handle, &requirements);
		this->allocation = device->getAllocator()->alloc(requirements, ALLOC_FLAG_NONE);
		vkBindImageMemory(device->getHandle(), handle, allocation.block->memory, allocation.offset);


		if (info.data != nullptr) {
			update(info.data);
		} else {
			device->getAllocator()->setImageLayout(this, desired_layout);
		}


		//image view
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.viewType = view_type;
		viewInfo.format = vk_format;
		viewInfo.subresourceRange.aspectMask |= info.flags & IMAGE_FLAG_DEPTH ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;


		image_views.resize(mip_levels);
		for (int i = 0; i < image_views.size(); ++i) {
			viewInfo.image = handle;
			viewInfo.subresourceRange.baseMipLevel = i;
			viewInfo.subresourceRange.levelCount = mip_levels - viewInfo.subresourceRange.baseMipLevel;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device->getHandle(), &viewInfo, nullptr, &image_views[i]) != VK_SUCCESS) {
				Log::error("failed to create texture image view!");
			}
		}


		if ((flags & IMAGE_FLAG_NO_SAMPLER) != IMAGE_FLAG_NO_SAMPLER) {
			VkSamplerCreateInfo samplerInfo{};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = (info.flags & IMAGE_FLAG_FILTER_NEAREST) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;//If the object is close to the camera,
			samplerInfo.minFilter = (info.flags & IMAGE_FLAG_FILTER_NEAREST) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;//If the object is further from the camera
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
			samplerInfo.minLod = 0.0f; // 0 when close the camera.
			samplerInfo.maxLod = static_cast<float>(mip_levels);
			samplerInfo.mipLodBias = 0.0f; // Optional

			if (vkCreateSampler(device->getHandle(), &samplerInfo, nullptr, &sampler_handle) != VK_SUCCESS) {
				Log::error("failed to create texture sampler!");
			}
		}
	}

	VK_Image::~VK_Image() {
		Log::debug("Delete image#" + std::to_string(id));
		if (sampler_handle != VK_NULL_HANDLE) {
			vkDestroySampler(device->getHandle(), sampler_handle, nullptr);
		}
		for (int i = 0; i < image_views.size(); ++i) {
			vkDestroyImageView(device->getHandle(), image_views[i], nullptr);
		}

		if (handle != VK_NULL_HANDLE) {
			vkDestroyImage(device->getHandle(), handle, nullptr);
		}
		device->getAllocator()->free(allocation);
	}

	const VkSampler &VK_Image::getSampler() const {
		return sampler_handle;
	}

	const VkImageView &VK_Image::getImageView(uint32_t mip_level) const {
		HB_ASSERT(mip_level < mip_levels, "Texture does not have mip level " + std::to_string(mip_level));
		return image_views[mip_level];
	}

	uint32_t VK_Image::getWidth() const {
		return width;
	}

	uint32_t VK_Image::getHeight() const {
		return height;
	}

	uint32_t VK_Image::getDepth() const {
		return depth;
	}

	const VkImageLayout VK_Image::getImageLayout() const {
		return layout;
	}

	const VkFormat VK_Image::getVkFormat() const {
		return vk_format;
	}

	const VkImage &VK_Image::getHandle() const {
		return handle;
	}

	void VK_Image::setImageLayout(VkImageLayout layout) {
		this->layout = layout;
	}

	vec3u VK_Image::getSize() const {
		return vec3u(width, height, depth);
	}

	const uint32_t VK_Image::bytePerPixel() const {
		return byte_per_pixel;
	}

	const VkImageLayout VK_Image::getDesiredLayout() const {
		return desired_layout;
	}

	uint32_t VK_Image::getMipLevelCount() const {
		return mip_levels;
	}


}
