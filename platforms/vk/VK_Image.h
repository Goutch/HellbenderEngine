#pragma once

#include <queue>
#include "core/resource/Image.h"
#include "vulkan/vulkan.h"
#include "array"
#include "VK_Types.h"

namespace HBE
{
    class VK_Context;
    class VK_Buffer;

    class VK_Fence;

    class VK_Renderer;

    class VK_Semaphore;
    class VK_Allocator;

    class VK_Image : public Image
    {
        VkImage handle = VK_NULL_HANDLE;
        VK_Context* context = nullptr;
        VkImageLayout layout;
        VkImageLayout desired_layout;
        VkImageView* image_views = nullptr;
        uint32_t image_view_count;
        VkSampler sampler_handle = VK_NULL_HANDLE;
        ImageInfo info;
        VkFormat vk_format;
        Allocation allocation;
        uint32_t width = 1, height = 1, depth = 1;
        uint32_t byte_per_pixel;
        uint32_t mip_levels;

    public:
        void alloc(VK_Context* context, const ImageInfo& info);
        void release();

        VK_Image& operator=(const VK_Image&) = delete;
        VK_Image(const VK_Image&) = delete;
        VK_Image(VK_Image&& other)noexcept;
        VK_Image() = default;
        ~VK_Image() = default;
        uint32_t getWidth() const override;

        uint32_t getHeight() const override;

        uint32_t getDepth() const override;

        vec3u getSize() const override;

        void update(const void* data, IMAGE_FORMAT format) override;

        void updateRegion(const void* data, uint32_t data_texel_count, ImageRegionUpdateInfo* update_info, uint32_t update_count) override;

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
