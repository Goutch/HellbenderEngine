#include "vulkan/vulkan.h"
#include "VK_Fence.h"
#include "VK_Device.h"
#include "core/utility/Log.h"

namespace HBE {
	VK_Fence::VK_Fence(const VK_Device &device) {
		this->device_handle = device.getHandle();
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		if (vkCreateFence(device_handle, &fenceInfo, nullptr, &handle) != VK_SUCCESS) {
			Log::error("failed to create fence!");
		}
	}

	VK_Fence::VK_Fence(const VK_Fence &&other) {
		this->device_handle = other.device_handle;
		this->handle = other.handle;
	}

	VK_Fence::~VK_Fence() {
		vkDestroyFence(device_handle, handle, nullptr);
	}

	const VkFence &VK_Fence::getHandle() const {
		return handle;
	}

	void VK_Fence::wait() const {
		vkWaitForFences(device_handle, 1, &handle, VK_TRUE, UINT64_MAX);
	}

	void VK_Fence::reset() const {
		vkResetFences(device_handle, 1, &handle);
	}

	bool VK_Fence::isSet() const {

		return vkGetFenceStatus(device_handle, handle) == VK_SUCCESS;
	}
}

