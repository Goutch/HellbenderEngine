#pragma once

#include "vulkan/vulkan.h"
#include "core/utility/Log.h"

namespace HBE {
	class VK_Device;

	struct Allocation;

	class VK_Buffer {
	public:
		enum Flags {
			NONE = 0,
			MAPPABLE = 1,
		};
	private:
		VK_Device *device;
		VkBuffer handle;
		Allocation *allocation;
		Flags flags;
	public:
		VK_Buffer &operator=(const VK_Buffer &) = delete;

		VK_Buffer(const VK_Buffer &) = delete;

		VK_Buffer(VK_Device *device, const void *data, VkDeviceSize size, VkBufferUsageFlags usage, Flags flags = NONE);

		VK_Buffer(VK_Device *device, VkDeviceSize size, VkBufferUsageFlags usage, Flags flags = NONE);

		~VK_Buffer();

		void update(const void *data);

		const VkBuffer &getHandle() const;

		void copy(VK_Buffer *other);

		Allocation &getAllocation();

		VkDeviceSize getSize() const;
	};

}

