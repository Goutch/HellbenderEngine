#pragma once

#include "core/interface/ROOTAccelerationStructureInterface.h"

namespace HBE {
	class VK_Device;

	class VK_TopLevelAccelerationStructure {
		VK_Context *context;
		VkAccelerationStructureKHR handle;
		VK_Buffer buffer;
		VkDeviceOrHostAddressConstKHR address{};
	public:
		void alloc(VK_Context *context,const  RootAccelerationStructureInfo& info);
		void release();
		bool allocated();
		VK_TopLevelAccelerationStructure() = default;
		~VK_TopLevelAccelerationStructure();
		VkDeviceOrHostAddressConstKHR getDeviceAddress() const;
		const VkAccelerationStructureKHR getHandle() const;
	};


}
