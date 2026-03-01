//
// Created by user on 10/1/2022.
//

#include "VK_StorageBuffer.h"
#include "vulkan/vulkan.h"
#include "VK_Buffer.h"

namespace HBE
{
    VK_StorageBuffer::VK_StorageBuffer(VK_Buffer& buffer, uint32_t size, uint32_t stride, uint32_t count): buffer(buffer)
    {
        this->count = count;
        this->stride = stride;
        this->size = stride * count;
    }

    void VK_StorageBuffer::update(const void* data)
    {
        buffer.update(data);
    }

    uint32_t VK_StorageBuffer::getStride() const
    {
        return stride;
    }

    VK_Buffer& VK_StorageBuffer::getBuffer() const
    {
        return buffer;
    }

    uint32_t VK_StorageBuffer::getCount() const
    {
        return count;
    }
}
