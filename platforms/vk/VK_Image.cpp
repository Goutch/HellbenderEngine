

#include "VK_Image.h"
#include "VK_Buffer.h"
#include "VK_Renderer.h"
#include "VK_CommandPool.h"
#include "VK_Fence.h"
#include "VK_Semaphore.h"
#include "VK_Utils.h"
namespace HBE {
	uint32_t VK_Image::current_id = 0;

	VkImageLayout VK_Image::chooseLayout() {
		if (flags & IMAGE_FLAG_DEPTH) {
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		} else if (flags & IMAGE_FLAG_RENDER_TARGET && !(flags & IMAGE_FLAG_SHADER_WRITE)) {
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		} else if (!(flags & IMAGE_FLAG_SHADER_WRITE)) {
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		return VK_IMAGE_LAYOUT_GENERAL;
	}

	void VK_Image::update(const void *data) {
		device->getAllocator()->update(*this, data, width, height, depth);
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
		vk_format =  VK_Utils::getVkFormat(format);
		byte_per_pixel = VK_Utils::getFormatStride(format);

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

		imageInfo.sharingMode = queues.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
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
			//image data is set right now
			update(info.data);
		} else if (info.flags & IMAGE_FLAG_RENDER_TARGET ||
				   info.flags & IMAGE_FLAG_DEPTH ||
				   info.flags & IMAGE_FLAG_SHADER_WRITE) {
			//image data will be set in the sahder
			device->getAllocator()->setImageLayout(this, desired_layout);
		} else {
			//image data must be set from a user setTexture(void *data) call
			device->getAllocator()->setImageLayout(this, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
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

	const VkSampler& VK_Image::getSampler() const {
		return sampler_handle;
	}

	const VkImageView& VK_Image::getImageView(uint32_t mip_level) const {
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

	VkImageLayout VK_Image::getImageLayout() const {
		return layout;
	}

	VkFormat VK_Image::getVkFormat() const {
		return vk_format;
	}

	const VkImage& VK_Image::getHandle() const {
		return handle;
	}

	void VK_Image::setImageLayout(VkImageLayout layout) {
		this->layout = layout;
	}

	vec3u VK_Image::getSize() const {
		return vec3u(width, height, depth);
	}

	uint32_t VK_Image::bytePerPixel() const {
		return byte_per_pixel;
	}

	VkImageLayout VK_Image::getDesiredLayout() const {
		return desired_layout;
	}

	uint32_t VK_Image::getMipLevelCount() const {
		return mip_levels;
	}


}

