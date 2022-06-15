#include "VK_Device.h"
#include "VK_AccelerationStructure.h"
#include "VK_Buffer.h"

namespace HBE {
	VK_AccelerationStructure::VK_AccelerationStructure(VK_Device &device) : device(device) {
		VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info{};
		acceleration_structure_create_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		acceleration_structure_create_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		acceleration_structure_create_info.createFlags = 0;

		acceleration_structure_create_info.offset = 0;
		device.vkCreateAccelerationStructureKHR(device.getHandle(), &acceleration_structure_create_info, nullptr, &handle);
	}

	VK_AccelerationStructure::~VK_AccelerationStructure() {
		device.vkDestroyAccelerationStructureKHR(device.getHandle(), handle, nullptr);
	}
}

