

#include "VK_Image.h"
#include "VK_Buffer.h"
#include "VK_Renderer.h"
#include "VK_CommandPool.h"
#include "VK_Fence.h"
#include "VK_Semaphore.h"

namespace HBE {
	uint32_t VK_Image::current_id=0;
    VkImageLayout VK_Image::chooseLayout() {
        if (flags & IMAGE_FLAG_RENDER_TARGET) {
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    void VK_Image::update(const void *data) {

        VK_Buffer staging_buffer = VK_Buffer(device, data, width * height * byte_per_pixel,
                                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT, ALLOC_FLAG_MAPPABLE);
        VK_Allocator *allocator = device->getAllocator();
        allocator->copy(staging_buffer.getHandle(), this, chooseLayout());
    }

    VK_Image::VK_Image(VK_Device *device, const TextureInfo &info) {
        this->device = device;
        this->width = info.width;
        this->height = info.height;
        this->depth = info.depth;
        this->format = info.format;
        this->flags = info.flags;
        this->id=current_id++;
        Log::debug("Create image#"+std::to_string(id));
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
        switch (format) {

            case IMAGE_FORMAT_R8:
                vk_format = VK_FORMAT_R8_SRGB;
                byte_per_pixel = 1;
                break;
            case IMAGE_FORMAT_RG8:
                vk_format = VK_FORMAT_R8G8_SRGB;
                byte_per_pixel = 2;
                break;
            case IMAGE_FORMAT_RGB8:
                vk_format = VK_FORMAT_R8G8B8_SRGB;
                byte_per_pixel = 3;
                break;
            case IMAGE_FORMAT_RGBA8:
                vk_format = VK_FORMAT_R8G8B8A8_SRGB;
                byte_per_pixel = 4;
                break;
            case IMAGE_FORMAT_R32F:
                vk_format = VK_FORMAT_R32_SFLOAT;
                byte_per_pixel = 4;
                break;
            case IMAGE_FORMAT_RG32F:
                vk_format = VK_FORMAT_R32G32_SFLOAT;
                byte_per_pixel = 4 * 2;
                break;
            case IMAGE_FORMAT_RGB32F:
                vk_format = VK_FORMAT_R32G32B32_SFLOAT;
                byte_per_pixel = 4 * 3;
                break;
            case IMAGE_FORMAT_RGBA32F:
                vk_format = VK_FORMAT_R32G32B32A32_SFLOAT;
                byte_per_pixel = 4 * 4;
                break;

        }

        layout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = type;
        imageInfo.extent.width = static_cast<uint32_t>(width);
        imageInfo.extent.height = static_cast<uint32_t>(height);
        imageInfo.extent.depth = static_cast<uint32_t>(depth);;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;

        imageInfo.format = vk_format;

        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional

        if (info.flags & IMAGE_FLAG_RENDER_TARGET)imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

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
            device->getAllocator()->setImageLayout(this, chooseLayout());
        }


        //image view
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = handle;
        viewInfo.viewType = view_type;
        viewInfo.format = vk_format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device->getHandle(), &viewInfo, nullptr, &view_hanlde) != VK_SUCCESS) {
            Log::error("failed to create texture image view!");
        }


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

    VK_Image::~VK_Image() {
    	Log::debug("Delete image#"+std::to_string(id));
        if (sampler_handle != VK_NULL_HANDLE) {
            vkDestroySampler(device->getHandle(), sampler_handle, nullptr);
        }
        if (view_hanlde != VK_NULL_HANDLE) {
            vkDestroyImageView(device->getHandle(), view_hanlde, nullptr);
        }
        if (handle != VK_NULL_HANDLE) {
            vkDestroyImage(device->getHandle(), handle, nullptr);
        }
		device->getAllocator()->free(allocation);
    }

    const VkSampler &VK_Image::getSampler() const {
        return sampler_handle;
    }

    const VkImageView &VK_Image::getImageView() const {
        return view_hanlde;
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


}
