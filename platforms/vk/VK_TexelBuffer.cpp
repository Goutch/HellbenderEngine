//
// Created by user on 10/12/2022.
//

#include "VK_TexelBuffer.h"
#include "VK_Allocator.h"
#include "VK_Utils.h"
#include "VK_Buffer.h"
#include "VK_Device.h"

namespace HBE
{
	VK_TexelBuffer::VK_TexelBuffer(VK_Device* device, const TexelBufferInfo& info)
	{
		HB_ASSERT(info.count > 0, "Texel buffer count must be greater than 0");
		this->device = device;

		count = info.count;


		size_t size = VK_Utils::getFormatStride(info.format) * count;
		buffer = new VK_Buffer(device,
			static_cast<VkDeviceSize>(size),
			VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
			info.preferred_memory_type_flags);


		VkBufferViewCreateInfo view_create_info = {};
		view_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
		view_create_info.buffer = buffer->getHandle();
		view_create_info.format = VK_Utils::getVkFormat(info.format);
		view_create_info.offset = 0;
		view_create_info.range = VK_WHOLE_SIZE;
		view_create_info.flags = 0;
		if (vkCreateBufferView(device->getHandle(), &view_create_info, nullptr, &view) != VK_SUCCESS)
		{
			Log::error("Failed to create buffer view");
		}
	}

	void VK_TexelBuffer::update(const void* data)
	{
		buffer->update(data);
	}

	VK_TexelBuffer::~VK_TexelBuffer()
	{
		vkDestroyBufferView(device->getHandle(), view, nullptr);
		delete buffer;
	}

	const VkBufferView& VK_TexelBuffer::getView() const
	{
		return view;
	}
}
