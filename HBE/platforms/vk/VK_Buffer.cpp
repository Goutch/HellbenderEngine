
#include "VK_Buffer.h"
#include "VK_Device.h"


namespace HBE {


	VK_Buffer::VK_Buffer(VK_Device *device, VkDeviceSize size, VkBufferUsageFlags usage, ALLOC_FLAGS flags) {
		this->device = device;
		this->size = size;
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = flags & ALLOC_FLAG_MAPPABLE ? usage : usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device->getHandle(), &bufferInfo, nullptr, &handle) != VK_SUCCESS) {
			Log::error("failed to create buffer!");
		}


		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(device->getHandle(), handle, &requirements);

		this->allocation = &device->getAllocator()->alloc(requirements, flags);
		vkBindBufferMemory(device->getHandle(), handle, allocation->block.memory, allocation->offset);
	}

	VK_Buffer::VK_Buffer(VK_Device *device, const void *data, VkDeviceSize size, VkBufferUsageFlags usage, ALLOC_FLAGS flags) : VK_Buffer(device, size, usage, flags) {
		update(data);
	}


	const VkBuffer &VK_Buffer::getHandle() const {
		return handle;
	}

	VK_Buffer::~VK_Buffer() {
		vkDestroyBuffer(device->getHandle(), handle, nullptr);
		device->getAllocator()->free(*allocation);
	}

	void VK_Buffer::copy(VK_Buffer *other) {
		device->getAllocator()->copy(other->getHandle(), this->getHandle(), size);
	}

	VkDeviceSize VK_Buffer::getSize() const {
		return size;
	}

	Allocation &VK_Buffer::getAllocation() {
		return *allocation;
	}

	void VK_Buffer::update(const void *data) {
		if (allocation->flags & ALLOC_FLAG_MAPPABLE) {
			void *buffer_data;
			vkMapMemory(device->getHandle(), allocation->block.memory, allocation->offset, allocation->size, 0, &buffer_data);
			memcpy(buffer_data, data, (size_t) size);
			vkUnmapMemory(device->getHandle(), allocation->block.memory);
		} else {
			VK_Buffer staging_buffer = VK_Buffer(device,
												 size,
												 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
												 ALLOC_FLAG_MAPPABLE);

			VkDeviceMemory &stagingBufferMemory = staging_buffer.getAllocation().block.memory;

			void *staging_buffer_data;
			vkMapMemory(device->getHandle(), stagingBufferMemory, staging_buffer.getAllocation().offset, allocation->size, 0, &staging_buffer_data);
			memcpy(staging_buffer_data, data, (size_t) allocation->size);
			vkUnmapMemory(device->getHandle(), stagingBufferMemory);
			copy(&staging_buffer);

		}

	}
}
