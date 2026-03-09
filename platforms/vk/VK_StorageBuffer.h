#pragma once

#include "VK_Buffer.h"
#include "core/interface/BufferInterface.h"

namespace HBE
{
    class VK_Context;
    class VK_Device;

    class VK_Buffer;

    class VK_StorageBuffer
    {
        uint32_t size;
        uint32_t stride;
        uint32_t count;
        VK_Buffer& buffer;

    public:
        VK_StorageBuffer(VK_Buffer& buffer, uint32_t size, uint32_t stride, uint32_t count);

        ~VK_StorageBuffer() = default;

        void update(const void* data);

        uint32_t getStride() const;
        VK_Buffer& getBuffer() const;

        uint32_t getCount() const;
    };
}
