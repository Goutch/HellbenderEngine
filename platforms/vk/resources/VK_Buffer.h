#pragma once
#include "vulkan/vulkan.h"
#include "core/interface/AllocatorInterface.h"
#include "HBE/platforms/vk/VK_Types.h"
#include "core/interface/BufferInterface.h"

namespace HBE {
    class VK_Allocator;
    class VK_Device;
    class VK_Context;

    struct VK_BufferInfo {
        void *data = nullptr;
        uint64_t size = 0;
        MEMORY_TYPE_FLAGS preferred_memory_type_flag = MEMORY_TYPE_FLAG_GPU_LOCAL;
        VkBufferUsageFlags usage = 0;
        uint64_t optional_custom_alignment = 0;
    };

    class VK_Buffer {
        VK_Allocator *allocator;
        VK_Device *device;
        VkBuffer handle = VK_NULL_HANDLE;
        Allocation allocation;
        VkDeviceSize size = 0;

    public:
        void alloc (VK_Context* context,const BufferInfo& info);
        void alloc(VK_Context *context, VK_BufferInfo &info);

        void release();

        VK_Buffer(VK_Buffer &) = delete;

        VK_Buffer &operator=(const VK_Buffer &) = delete;

        VK_Buffer(VK_Buffer &&) noexcept;

        VK_Buffer() noexcept = default;

        VK_Buffer &operator=(VK_Buffer &&) noexcept;

        bool allocated();

        void reset();

        void update(const void *data);

        VkBuffer getHandle() const;

        void copy(VK_Buffer *other);

        const Allocation &getAllocation() const;

        VkDeviceSize getSize() const;

        VkDeviceOrHostAddressConstKHR getDeviceAddress() const;

        void map(void *data);
    };
}
