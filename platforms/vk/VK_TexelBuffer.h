#pragma once

#include "VK_Buffer.h"
#include "vulkan/vulkan.h"

namespace HBE {
    struct TexelBufferInfo;
    class VK_Context;

    class VK_TexelBuffer {
        VK_Context *context;
        uint32_t count;
        VkBufferView view;
        VK_Buffer buffer;

    public:
        VK_TexelBuffer() = default;

        ~VK_TexelBuffer() = default;

        VK_TexelBuffer(VK_TexelBuffer &other) = delete;

        void alloc(VK_Context *context, const TexelBufferInfo &info);

        void release();

        bool allocated();

        void update(const void *data);

        const VkBufferView &getView() const;
    };
}
