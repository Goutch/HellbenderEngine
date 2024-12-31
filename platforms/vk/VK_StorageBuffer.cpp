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
		if (this->deleteOnDestruction)
			delete buffer;
	}

	void VK_StorageBuffer::update(const void *data) {
		buffer->update(data);
	}

	const VK_Buffer &VK_StorageBuffer::getBuffer() const {
		return *buffer;
	}

	uint32_t VK_StorageBuffer::getStride() const {
		return stride;
	}

	uint32_t VK_StorageBuffer::getCount() const {
		return count;
	}

	void VK_StorageBuffer::update(const void *data, size_t size, size_t offset) {
		buffer->update(data, static_cast<VkDeviceSize>(size * stride), static_cast<VkDeviceSize>(offset * stride));
	}

	VK_StorageBuffer::VK_StorageBuffer(VK_Device *device, VK_Buffer *buffer, uint32_t stride, uint32_t count, bool deleteOnDestruction) {
		this->buffer = buffer;
		this->stride = stride;
		this->count = count;
		this->size = stride * count;
		this->deleteOnDestruction = deleteOnDestruction;
	}

}

