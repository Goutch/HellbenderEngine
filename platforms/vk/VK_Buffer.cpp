
#include "VK_Buffer.h"
#include "VK_Device.h"

#include "cstring"
#include "array"

namespace HBE {


	VK_Buffer::VK_Buffer(VK_Device *device, VkDeviceSize size, VkBufferUsageFlags usage, ALLOC_FLAGS flags, VkDeviceSize custom_alignment) {
		this->device = device;
		alloc(size, usage, flags, custom_alignment);
	}

	VK_Buffer::VK_Buffer(VK_Device *device) {
		this->device = device;
	}

	VK_Buffer::VK_Buffer(VK_Device *device, const void *data, VkDeviceSize size, VkBufferUsageFlags usage, ALLOC_FLAGS flags, VkDeviceSize custom_alignment) : VK_Buffer(device, size, usage, flags,
	                                                                                                                                                                     custom_alignment) {
		update(data);
	}


	VkBuffer VK_Buffer::getHandle() const {
		return handle;
	}

	VK_Buffer::~VK_Buffer() {
		if (allocated()) {
			vkDestroyBuffer(device->getHandle(), handle, nullptr);
			device->getAllocator()->free(allocation);
		}
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

	void VK_Buffer::update(const void *data, VkDeviceSize size, VkDeviceSize offset) {
		device->getAllocator()->update(*this, data, size, offset);
	}

	void VK_Buffer::alloc(VkDeviceSize size, VkBufferUsageFlags usage, ALLOC_FLAGS flags, VkDeviceSize custom_alignment) {
		HB_ASSERT(size > 0, "Buffer size must be greater than 0");
		this->size = size;
		std::vector<uint32_t> queues = {device->getQueue(QUEUE_FAMILY_GRAPHICS).getFamilyIndex()};
		if (device->hasQueue(QUEUE_FAMILY_TRANSFER))device->getQueue(QUEUE_FAMILY_TRANSFER).getFamilyIndex();
		if (device->hasQueue(QUEUE_FAMILY_COMPUTE)) device->getQueue(QUEUE_FAMILY_COMPUTE).getFamilyIndex();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = flags & ALLOC_FLAG_MAPPABLE ? usage : usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = queues.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
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
		requirements.alignment = std::max(requirements.alignment, custom_alignment);

		this->allocation = device->getAllocator()->alloc(requirements, flags);
		vkBindBufferMemory(device->getHandle(), handle, allocation.block->memory, allocation.offset);
	}

	bool VK_Buffer::allocated() {
		return size > 0;
	}

	void VK_Buffer::alloc(const void *data, VkDeviceSize size, VkBufferUsageFlags usage, ALLOC_FLAGS flags, VkDeviceSize custom_alignment) {
		alloc(size, usage, flags, custom_alignment);
		update(data);
	}
}
