#pragma once

#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Device;

	class VK_AccelerationStructure {

		VkAccelerationStructureKHR handle;
		VK_Device &device;
	public:
		VK_AccelerationStructure(VK_Device &device);
		~VK_AccelerationStructure();
	};
}



