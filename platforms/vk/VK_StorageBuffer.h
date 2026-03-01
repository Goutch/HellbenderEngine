#pragma once

#include "VK_Buffer.h"
#include "core/resource/StorageBuffer.h"

namespace HBE
{
    class VK_Context;
    class VK_Device;

    class VK_Buffer;

    class VK_StorageBuffer : public StorageBuffer
    {
        uint32_t size;
        uint32_t stride;
        uint32_t count;
        VK_Buffer& buffer;

    public:

        VK_StorageBuffer(VK_Buffer& buffer, uint32_t size, uint32_t stride, uint32_t count);

        ~VK_StorageBuffer() override = default;

        void update(const void* data) override;

        uint32_t getStride() const override;
        VK_Buffer& getBuffer() const;

        uint32_t getCount() const override;
    };
}
