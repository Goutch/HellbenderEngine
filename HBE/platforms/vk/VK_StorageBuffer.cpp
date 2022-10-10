//
// Created by user on 10/1/2022.
//

#include "VK_StorageBuffer.h"
#include "vulkan/vulkan.h"
#include "VK_Buffer.h"

namespace HBE {

	VK_StorageBuffer::VK_StorageBuffer(VK_Device *device, const StorageBufferInfo &info) {

		ALLOC_FLAGS flags = ALLOC_FLAG_NONE;
		if ((info.flags & STORAGE_BUFFER_FLAG_MAPPABLE) != 0) {
			flags |= ALLOC_FLAG_MAPPABLE;
		}
		count = info.count;
		stride = info.stride;
		size = stride * count;
		buffer = new VK_Buffer(device, static_cast<VkDeviceSize>(size), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, flags);
	}

	VK_StorageBuffer::~VK_StorageBuffer() {
		delete buffer;
	}

	void VK_StorageBuffer::update(const void *data) {
		buffer->update(data);
	}

	const VK_Buffer &VK_StorageBuffer::getBuffer() const {
		return *buffer;
	}

	uint32_t VK_StorageBuffer::getStride() const {
		return count;
	}

	uint32_t VK_StorageBuffer::getCount() const {
		return stride;
	}
}

