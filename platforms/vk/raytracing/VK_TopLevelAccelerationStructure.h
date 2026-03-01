#pragma once

#include "core/resource/raytracing/AccelerationStructure.h"

namespace HBE {
	class VK_Device;

	class VK_TopLevelAccelerationStructure : public RootAccelerationStructure {
		VK_Context *context;
		VkAccelerationStructureKHR handle;
		VK_Buffer buffer;
		VkDeviceOrHostAddressConstKHR address{};
	public:
		VK_TopLevelAccelerationStructure(VK_Context *context,const  RootAccelerationStructureInfo& info);
		~VK_TopLevelAccelerationStructure();
		VkDeviceOrHostAddressConstKHR getDeviceAddress() const;
		const VkAccelerationStructureKHR getHandle() const;
	};


}