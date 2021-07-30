
#include "VK_Buffer.h"
#include "VK_Device.h"
#include "VK_Allocator.h"

namespace HBE {
	VkMemoryPropertyFlags choseProperties(VK_Buffer::Flags flags) {
		VkMemoryPropertyFlags properties = 0;
		if (flags & VK_Buffer::MAPPABLE) {
			properties |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		} else {
			properties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}

		return properties;
	}

	VK_Buffer::VK_Buffer(VK_Device *device, VkDeviceSize size, VkBufferUsageFlags usage, Flags flags) {
		this->device = device;
		this->flags = flags;

		VkMemoryPropertyFlags properties = choseProperties(flags);
		if (flags & MAPPABLE) {
			this->allocation = &device->getAllocator().alloc(handle, size, usage, properties);
		} else {
			this->allocation = &device->getAllocator().alloc(handle, size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, properties);
		}
	}

	VK_Buffer::VK_Buffer(VK_Device *device, const void *data, VkDeviceSize size, VkBufferUsageFlags usage, Flags flags) : VK_Buffer(device, size, usage, flags) {
		update(data);
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


	void VK_Buffer::update(const void *data) {
		if (flags & MAPPABLE) {
			void *buffer_data;
			vkMapMemory(device->getHandle(), allocation->block.memory, allocation->offset, allocation->size, 0, &buffer_data);
			memcpy(buffer_data, data, (size_t) allocation->size);
			vkUnmapMemory(device->getHandle(), allocation->block.memory);
		} else {
			VK_Buffer staging_buffer = VK_Buffer(device,
												 allocation->size,
												 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
												 MAPPABLE);

			VkDeviceMemory &stagingBufferMemory = staging_buffer.getAllocation().block.memory;

			void *staging_buffer_data;
			vkMapMemory(device->getHandle(), stagingBufferMemory, staging_buffer.getAllocation().offset, allocation->size, 0, &staging_buffer_data);
			memcpy(staging_buffer_data, data, (size_t) allocation->size);
			vkUnmapMemory(device->getHandle(), stagingBufferMemory);
			copy(&staging_buffer);

		}

	}
}
