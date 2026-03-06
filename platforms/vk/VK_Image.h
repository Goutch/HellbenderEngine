#pragma once

#include <queue>
#include "core/resource/Image.h"
#include "vulkan/vulkan.h"
#include "array"
#include "VK_Types.h"

namespace HBE {
    class VK_Context;
    class VK_Buffer;

    class VK_Fence;

    class VK_Renderer;

    class VK_Semaphore;
    class VK_Allocator;

    class VK_Image {
        VkImage handle;
        VK_Context *context = nullptr;
        VkImageLayout layout;
        VkImageLayout desired_layout;
        VkImageView *image_views = nullptr;
        uint32_t image_view_count;
        VkSampler sampler_handle;
        VkFormat vk_format;
        uint32_t width = 1, height = 1, depth = 1;
        uint32_t byte_per_pixel;
        uint32_t mip_levels;
        IMAGE_FORMAT format = IMAGE_FORMAT_UNDEFINED;
        IMAGE_FLAGS flags = IMAGE_FLAG_NONE;
        bool m_allocated = false;
        Allocation allocation{};
    public:
        VK_Image() = default;

        void alloc(VK_Context *context, const ImageInfo &info);

        void release();
        VK_Image& operator=(const VK_Image&) = delete;
        VK_Image(VK_Image&& other)noexcept;

        uint32_t getWidth() const;

        uint32_t getHeight() const;

        uint32_t getDepth() const;

        vec3u getSize() const;

        void update(const void *data);

        void updateRegion(const void *data, uint32_t data_texel_count, ImageRegionUpdateInfo *update_info, uint32_t update_count);

        const VkSampler &getSampler() const;

        const VkImage &getHandle() const;

        const VkImageView &getImageView(uint32_t mip_level = 0) const;

        uint32_t getMipLevelCount() const;

        VkFormat getVkFormat() const;

        VkImageLayout getImageLayout() const;

        VkImageLayout getDesiredLayout() const;

        uint32_t bytePerPixel() const;

        void setImageLayout(VkImageLayout layout);

        bool allocated();
    private:
        VkImageLayout chooseLayout();
    };
}
