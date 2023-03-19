
#include "VK_Buffer.h"
#include "VK_Device.h"

#include "cstring"
#include "array"

namespace HBE {


	VK_Buffer::VK_Buffer(VK_Device *device, VkDeviceSize size, VkBufferUsageFlags usage, ALLOC_FLAGS flags) {
		this->device = device;
		this->size = size;
		std::array<uint32_t, 3> queues = {device->getQueue(QUEUE_FAMILY_GRAPHICS).getFamilyIndex(),
										  device->getQueue(QUEUE_FAMILY_TRANSFER).getFamilyIndex(),
										  device->getQueue(QUEUE_FAMILY_COMPUTE).getFamilyIndex()};
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = flags & ALLOC_FLAG_MAPPABLE ? usage : usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
		bufferInfo.queueFamilyIndexCount = queues.size();
		bufferInfo.pQueueFamilyIndices = queues.data();

		if (vkCreateBuffer(device->getHandle(), &bufferInfo, nullptr, &handle) != VK_SUCCESS) {
			Log::error("failed to create buffer!");
		}

#ifdef PRINT_BUFFER_CREATION
		void* handle_ptr = handle;
		printf("create buffer:%p\n",handle_ptr);
#endif
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(device->getHandle(), handle, &requirements);

		this->allocation = device->getAllocator()->alloc(requirements, flags);
		vkBindBufferMemory(device->getHandle(), handle, allocation.block->memory, allocation.offset);
	}

	VK_Buffer::VK_Buffer(VK_Device *device, const void *data, VkDeviceSize size, VkBufferUsageFlags usage, ALLOC_FLAGS flags) : VK_Buffer(device, size, usage, flags) {
		update(data);
	}


	VkBuffer VK_Buffer::getHandle() const {
		return handle;
	}

	VK_Buffer::~VK_Buffer() {
		vkDestroyBuffer(device->getHandle(), handle, nullptr);
		device->getAllocator()->free(allocation);
	}

	void VK_Buffer::copy(VK_Buffer *other) {
		device->getAllocator()->copy(other->getHandle(), this->getHandle(), size);
	}

	VkDeviceSize VK_Buffer::getSize() const {
		return size;
	}

	const Allocation &VK_Buffer::getAllocation() const {
		return allocation;
	}

	void VK_Buffer::update(const void *data) {
		device->getAllocator()->update(*this, data, size);
	}

	VkDeviceOrHostAddressConstKHR VK_Buffer::getDeviceAddress() const {
		VkBufferDeviceAddressInfo bufferDeviceAddressInfo{};
		bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		bufferDeviceAddressInfo.buffer = handle;

		VkDeviceOrHostAddressConstKHR buffer_address{};
		buffer_address.deviceAddress = device->vkGetBufferDeviceAddressKHR(device->getHandle(), &bufferDeviceAddressInfo);
		return buffer_address;
	}

	void VK_Buffer::map(void *data) {
		if (allocation.flags | ALLOC_FLAG_MAPPABLE) {
			vkMapMemory(device->getHandle(), allocation.block->memory, allocation.offset, allocation.size, 0, &data);
		} else {
			Log::error("Buffer is not mappable");
		}
	}
}
