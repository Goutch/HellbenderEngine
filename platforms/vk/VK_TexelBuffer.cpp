//
// Created by user on 10/12/2022.
//

#include "VK_TexelBuffer.h"
#include "VK_Allocator.h"
#include "VK_Utils.h"
#include "VK_Buffer.h"
#include "VK_Context.h"
#include "VK_Device.h"

namespace HBE
{
    void VK_TexelBuffer::alloc(VK_Context* context, const TexelBufferInfo& info)
    {
        HB_ASSERT(info.count > 0, "Texel buffer count must be greater than 0");
        this->context = context;

        BufferInfo buffer_info;
        buffer_info.size = VK_Utils::getFormatStride(info.format) * count;
        buffer_info.usage = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
        buffer_info.memory_type_flags = info.preferred_memory_type_flags;

        buffer.alloc(context, buffer_info);

        VkBufferViewCreateInfo view_create_info = {};
        view_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
        view_create_info.buffer = buffer.getHandle();
        view_create_info.format = VK_Utils::getVkFormat(info.format);
        view_create_info.offset = 0;
        view_create_info.range = VK_WHOLE_SIZE;
        view_create_info.flags = 0;
        if (vkCreateBufferView(context->device.getHandle(), &view_create_info, nullptr, &view) != VK_SUCCESS)
        {
            Log::error("Failed to create buffer view");
        }
    }

    void VK_TexelBuffer::release()
    {
        vkDestroyBufferView(context->device.getHandle(), view, nullptr);
        buffer.release();
    }

    void VK_TexelBuffer::update(const void* data)
    {
        buffer.update(data);
    }

    const VkBufferView& VK_TexelBuffer::getView() const
    {
        return view;
    }
}
