
#include "VK_Buffer.h"
#include "VK_Device.h"
#include "VK_Allocator.h"

namespace HBE {
	VK_Buffer::VK_Buffer(VK_Device *device,const void *data, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
		this->device = device;
		if (properties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
			this->allocation = &device->getAllocator().alloc(handle, size, usage|VK_BUFFER_USAGE_TRANSFER_DST_BIT, properties);
			VK_Buffer staging_buffer = VK_Buffer(device,
												 data,
												 size,
												 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
												 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			VkDeviceMemory &stagingBufferMemory = staging_buffer.getAllocation().block.memory;
			void *staging_buffer_data;
			vkMapMemory(device->getHandle(), stagingBufferMemory, staging_buffer.getAllocation().offset, size, 0, &staging_buffer_data);
			memcpy(staging_buffer_data, data, (size_t) size);
			vkUnmapMemory(device->getHandle(), stagingBufferMemory);
			copy(&staging_buffer);
		}
		else
		{
			this->allocation = &device->getAllocator().alloc(handle, size, usage, properties);
		}

	}


	const VkBuffer &VK_Buffer::getHandle() const {
		return handle;
	}

	VK_Buffer::~VK_Buffer() {
		device->getAllocator().free(handle, *allocation);
	}

	void VK_Buffer::copy(VK_Buffer *other) {
		device->getAllocator().copy(other, this);
	}

	VkDeviceSize VK_Buffer::getSize() const {
		return allocation->size;
	}

	Allocation &VK_Buffer::getAllocation() {
		return *allocation;
	}
}