#pragma once

#include "vulkan/vulkan.h"
#include "core/utility/Log.h"
#include "VK_Allocator.h"

namespace HBE {
	class VK_Device;

	struct Allocation;

	class VK_Buffer {
	private:
		VK_Device *device;
		VkBuffer handle;
		Allocation allocation;
		VkDeviceSize size = 0;
	public:
		VK_Buffer &operator=(const VK_Buffer &) = delete;

		VK_Buffer(const VK_Buffer &) = delete;

		VK_Buffer(VK_Device *device);

		VK_Buffer(VK_Device *device, const void *data, VkDeviceSize size, VkBufferUsageFlags usage, MEMORY_TYPE_FLAGS flags = MEMORY_TYPE_FLAG_NONE, VkDeviceSize custom_alignment = 0);

		VK_Buffer(VK_Device *device, VkDeviceSize size, VkBufferUsageFlags usage, MEMORY_TYPE_FLAGS flags = MEMORY_TYPE_FLAG_NONE, VkDeviceSize custom_alignment = 0);

		~VK_Buffer();

		void alloc(VkDeviceSize size, VkBufferUsageFlags usage, MEMORY_TYPE_FLAGS flags = MEMORY_TYPE_FLAG_NONE, VkDeviceSize custom_alignment = 0);

		void alloc(const void *data, VkDeviceSize size, VkBufferUsageFlags usage, MEMORY_TYPE_FLAGS flags = MEMORY_TYPE_FLAG_NONE, VkDeviceSize custom_alignment = 0);

		void update(const void *data);

		void update(const void *data, VkDeviceSize size, VkDeviceSize offset = 0);

		VkBuffer getHandle() const;

		void copy(VK_Buffer *other);

		const Allocation &getAllocation() const;

		VkDeviceSize getSize() const;

		VkDeviceOrHostAddressConstKHR getDeviceAddress() const;

		void map(void *data);

		bool allocated();
	};

}

