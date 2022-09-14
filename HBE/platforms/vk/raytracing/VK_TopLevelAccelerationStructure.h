#pragma once

#include "core/resource/raytracing/AccelerationStructure.h"

namespace HBE {
	class VK_Device;

	class VK_TopLevelAccelerationStructure : public RootAccelerationStructure {
		VK_Device *device;
		VkAccelerationStructureKHR handle;
		VK_Buffer *buffer;
		VkDeviceOrHostAddressConstKHR address{};
	public:
		VK_TopLevelAccelerationStructure(VK_Device *device, RootAccelerationStructureInfo info);
		~VK_TopLevelAccelerationStructure() override;
		VkDeviceOrHostAddressConstKHR getDeviceAddress() const;
		const VkAccelerationStructureKHR getHandle() const;
	};


}