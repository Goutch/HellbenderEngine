#pragma once

#include <core/resource/TexelBuffer.h>

#include "VK_Buffer.h"
#include "vulkan/vulkan.h"

namespace HBE
{
    class VK_Context;

    class VK_TexelBuffer : public TexelBuffer
    {
        VK_Context* context;
        uint32_t count;
        VkBufferView view;
        VK_Buffer buffer;

    public:
        VK_TexelBuffer() = default;
        ~VK_TexelBuffer() final = default;

        TexelBuffer& operator =(TexelBuffer& other) = delete;
        VK_TexelBuffer(TexelBuffer& other) = delete;
        void alloc(VK_Context* context, const TexelBufferInfo& info);
        void release();
        void update(const void* data) final;

        const VkBufferView& getView() const;
    };
}
