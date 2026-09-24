#pragma once

#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Context;

	class VK_Semaphore {
		VkSemaphore handle = VK_NULL_HANDLE;
		VK_Context *context;
	public:
		VK_Semaphore() = default;

		void alloc(VK_Context &context);

		bool allocated();

		void release();

		~VK_Semaphore() = default;

		const VkSemaphore &getHandle() const;
	};
}
