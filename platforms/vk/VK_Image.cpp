#include "VK_Image.h"
#include "VK_Buffer.h"
#include "VK_Renderer.h"
#include "VK_CommandPool.h"
#include "VK_Fence.h"
#include "VK_Semaphore.h"
#include "VK_Utils.h"
#include "core/resource/Resources.h"

namespace HBE {
    uint32_t VK_Image::current_id = 0;

    std::string formatToString(IMAGE_FORMAT format) {
        switch (format) {
            case IMAGE_FORMAT_R8_UINT:
                return "R8_UINT";
            case IMAGE_FORMAT_RG8_UINT:
                return "RG8_UINT";
            case IMAGE_FORMAT_RGB8_UINT:
                return "RGB8_UINT";
            case IMAGE_FORMAT_RGBA8_UINT:
                return "RGBA8_UINT";
            case IMAGE_FORMAT_R8:
                return "R8";
            case IMAGE_FORMAT_RG8:
                return "RG8";
            case IMAGE_FORMAT_RGB8:
                return "RGB8";
            case IMAGE_FORMAT_RGBA8:
                return "RGBA8";
            case IMAGE_FORMAT_R32_UINT:
                return "R32_UINT";
            case IMAGE_FORMAT_R32F:
                return "R32F";
            case IMAGE_FORMAT_RG32F:
                return "RG32F";
            case IMAGE_FORMAT_RGB32F:
                return "RGB32F";
            case IMAGE_FORMAT_RGBA32F:
                return "RGBA32F";
            case IMAGE_FORMAT_DEPTH32F:
                return "DEPTH32F";
            case IMAGE_FORMAT_DEPTH32f_STENCIL8U:
                return "DEPTH32f_STENCIL8U";
            case IMAGE_FORMAT_DEPTH24f_STENCIL8U:
                return "DEPTH24f_STENCIL8U";
            case IMAGE_FORMAT_SBGRA8_NON_LINEAR:
                return "SBGRA8_NON_LINEAR";
            case IMAGE_FORMAT_SR8_NON_LINEAR:
                return "SR8_NON_LINEAR";
            case IMAGE_FORMAT_SRG8_NON_LINEAR:
                return "SRG8_NON_LINEAR";
            case IMAGE_FORMAT_SRGB8_NON_LINEAR:
                return "SRGB8_NON_LINEAR";
            case IMAGE_FORMAT_SRGBA8_NON_LINEAR:
                return "SRGBA8_NON_LINEAR";
            case IMAGE_FORMAT_R8_UNORM:
                return "R8_UNORM";
            case IMAGE_FORMAT_RG8_UNORM:
                return "RG8_UNORM";
            case IMAGE_FORMAT_RGB8_UNORM:
                return "RGB8_UNORM";
            case IMAGE_FORMAT_RGBA8_UNORM:
                return "RGBA8_UNORM";
            case IMAGE_FORMAT_R8_SNORM:
                return "R8_SNORM";
            case IMAGE_FORMAT_RG8_SNORM:
                return "RG8_SNORM";
            case IMAGE_FORMAT_RGB8_SNORM:
                return "RGB8_SNORM";
            case IMAGE_FORMAT_RGBA8_SNORM:
                return "RGBA8_SNORM";
            case IMAGE_FORMAT_R16F:
                return "R16F";
            case IMAGE_FORMAT_RG16F:
                return "RG16F";
            case IMAGE_FORMAT_RGB16F:
                return "RGB16F";
            case IMAGE_FORMAT_RGBA16F:
                return "RGBA16F";
            case IMAGE_FORMAT_R16_UINT:
                return "R16_UINT";
            case IMAGE_FORMAT_RG16_UINT:
                return "RG16_UINT";
            case IMAGE_FORMAT_RGB16_UINT:
                return "RGB16_UINT";
            case IMAGE_FORMAT_RGBA16_UINT:
                return "RGBA16_UINT";
            case IMAGE_FORMAT_R16_UNORM:
                return "R16_UNORM";
            case IMAGE_FORMAT_RG16_UNORM:
                return "RG16_UNORM";
            case IMAGE_FORMAT_RGB16_UNORM:
                return "RGB16_UNORM";
            case IMAGE_FORMAT_RGBA16_UNORM:
                return "RGBA16_UNORM";
            case IMAGE_FORMAT_R16_SNORM:
                return "R16_SNORM";
            case IMAGE_FORMAT_RG16_SNORM:
                return "RG16_SNORM";
            case IMAGE_FORMAT_RGB16_SNORM:
                return "RGB16_SNORM";
            case IMAGE_FORMAT_RGBA16_SNORM:
                return "RGBA16_SNORM";
            default:
                return "UNKNOWN";
        }
    }

    VkImageLayout VK_Image::chooseLayout() {
        if (info.flags & IMAGE_FLAG_DEPTH) {
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        } else if (info.flags & IMAGE_FLAG_RENDER_TARGET && !(info.flags & IMAGE_FLAG_SHADER_WRITE)) {
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        } else if (!(info.flags & IMAGE_FLAG_SHADER_WRITE) && !(info.flags & IMAGE_FLAG_NO_SAMPLER)) {
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        return VK_IMAGE_LAYOUT_GENERAL;
    }

    void VK_Image::update(const void *data, IMAGE_FORMAT format) {
        if (format != IMAGE_FORMAT_UNDEFINED && info.format != format) {
            Log::debug(
                "transition image data from " + formatToString(format) + " to " + formatToString(info.format) +
                " staging image creation");
            ImageInfo info = {};
            info.width = width;
            info.height = height;
            info.depth = depth;
            info.format = format;
            info.data_format = format;
            info.flags = IMAGE_FLAG_NO_SAMPLER;
            info.data = data;
            VK_Image *temp_image = (VK_Image *) Resources::createImage(info);

            FreeRequest staging_allocation = {};
            staging_allocation.allocation = temp_image->allocation;
            staging_allocation.vk_image = temp_image->getHandle();
            staging_allocation.fence = device->getAllocator()->blitImage(*temp_image, *this)->getHandle();
            //do not wait for blit to finish, delete the staging buffer later.
            device->getAllocator()->freeLater(staging_allocation);
        } else {
            device->getAllocator()->update(*this, data, width, depth, height);
        }
    }


    void VK_Image::updateRegion(const void *data, uint32_t data_texel_count, ImageRegionUpdateInfo *update_info,
                                uint32_t update_count) {
        device->getAllocator()->updateRegions(*this, data, data_texel_count, update_info, update_count);
    }


    static VkSamplerAddressMode adressModeToVKAddressMode(IMAGE_SAMPLER_ADDRESS_MODE adress_mode) {
        switch (adress_mode) {
            case IMAGE_SAMPLER_ADDRESS_MODE_REPEAT:
                return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case IMAGE_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
                return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case IMAGE_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case IMAGE_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            case IMAGE_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE:
                return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        }
    }


    VK_Image::VK_Image(VK_Device *device, const ImageInfo &info) {
        this->device = device;
        this->width = info.width;
        this->height = info.height;
        this->depth = info.depth;
        this->info = info;
        this->id = current_id++;
        this->mip_levels = info.generate_mip_maps
                               ? static_cast<uint32_t>(std::floor(
                                     std::log2(std::max(std::max(info.width, info.height), info.depth)))) + 1
                               : 1;

        desired_layout = chooseLayout();

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

        vk_format = VK_Utils::getVkFormat(info.format);
        byte_per_pixel = VK_Utils::getFormatStride(info.format);

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
        imageInfo.flags = 0; // Optional		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        imageInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.usage |= (info.flags & IMAGE_FLAG_NO_SAMPLER) ? 0 : VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.usage |= (info.flags & IMAGE_FLAG_RENDER_TARGET) ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0;
        imageInfo.usage |= (info.flags & IMAGE_FLAG_DEPTH) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : 0;
        imageInfo.usage |= (info.flags & IMAGE_FLAG_SHADER_WRITE || info.flags & IMAGE_FLAG_NO_SAMPLER)
                               ? VK_IMAGE_USAGE_STORAGE_BIT
                               : 0;
        imageInfo.usage |= (info.generate_mip_maps ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0);
        if (vkCreateImage(device->getHandle(), &imageInfo, nullptr, &handle) != VK_SUCCESS) {
            Log::error("failed to create image!");
        }
#ifdef DEBUG_MODE
        Log::debug(
            "Created image#" + std::to_string(id) + " handle: " + VK_Utils::handleToString(handle) + " format " +
            formatToString(info.format));
#endif
        VkMemoryRequirements requirements;
        vkGetImageMemoryRequirements(device->getHandle(), handle, &requirements);
        this->allocation = device->getAllocator()->alloc(requirements, MEMORY_TYPE_FLAG_NONE);
        vkBindImageMemory(device->getHandle(), handle, allocation.block->memory, allocation.offset);

        if (info.data != nullptr) {
            //image data is set right now
            update(info.data, info.data_format);
        } else if (info.flags & IMAGE_FLAG_RENDER_TARGET ||
                   info.flags & IMAGE_FLAG_DEPTH ||
                   info.flags & IMAGE_FLAG_SHADER_WRITE) {
            //image data will be set in the shader
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
        viewInfo.subresourceRange.aspectMask |= info.flags & IMAGE_FLAG_DEPTH
                                                    ? VK_IMAGE_ASPECT_DEPTH_BIT
                                                    : VK_IMAGE_ASPECT_COLOR_BIT;


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


        if ((info.flags & IMAGE_FLAG_NO_SAMPLER) != IMAGE_FLAG_NO_SAMPLER) {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = (info.sampler_info.filter == IMAGE_SAMPLER_FILTER_TYPE_NEAREST)
                                        ? VK_FILTER_NEAREST
                                        : VK_FILTER_LINEAR; //If the object is close to the camera,
            samplerInfo.minFilter = (info.sampler_info.filter == IMAGE_SAMPLER_FILTER_TYPE_NEAREST)
                                        ? VK_FILTER_NEAREST
                                        : VK_FILTER_LINEAR; //If the object is further from the camera
            VkFormatProperties format_properties;
            vkGetPhysicalDeviceFormatProperties(device->getPhysicalDevice().getHandle(), vk_format, &format_properties);
            if ((format_properties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) == 0) {
                samplerInfo.magFilter = VK_FILTER_NEAREST;
                samplerInfo.minFilter = VK_FILTER_NEAREST;
                Log::warning(
                    formatToString(info.format) +
                    " image format does not support linear filtering, using nearest instead");
            }

            samplerInfo.addressModeU = adressModeToVKAddressMode(info.sampler_info.address_mode);
            samplerInfo.addressModeV = adressModeToVKAddressMode(info.sampler_info.address_mode);
            samplerInfo.addressModeW = adressModeToVKAddressMode(info.sampler_info.address_mode);

            samplerInfo.anisotropyEnable = device->getPhysicalDevice().getFeatures().samplerAnisotropy;
            samplerInfo.maxAnisotropy = device->getPhysicalDevice().getProperties().limits.maxSamplerAnisotropy;

            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;

            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;


            if (info.generate_mip_maps) {
                samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                samplerInfo.minLod = 0.0f; // 0 when close the camera.
                samplerInfo.maxLod = static_cast<float>(mip_levels);
                samplerInfo.mipLodBias = 0.0f; // Optional
            }

            if (vkCreateSampler(device->getHandle(), &samplerInfo, nullptr, &sampler_handle) != VK_SUCCESS) {
                Log::error("failed to create texture sampler!");
            }

            Log::debug(
                "Created sampler " + VK_Utils::handleToString(sampler_handle) + " with mode:" +
                VK_Utils::samplerModeToString(info.sampler_info.address_mode) +
                " | filter:" + VK_Utils::samplerFilterTypeToString(info.sampler_info.filter));
        }
    }


    VK_Image::~VK_Image() {
        Log::debug(
            std::to_string((uint64_t) handle) + "|Delete image#" + std::to_string(id) + "handle: " +
            VK_Utils::handleToString(handle));
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

    VkImageLayout VK_Image::getImageLayout() const {
        return layout;
    }

    VkFormat VK_Image::getVkFormat() const {
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
