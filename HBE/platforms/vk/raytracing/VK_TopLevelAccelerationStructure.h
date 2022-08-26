#pragma once

#include "core/resource/raytracing/AccelerationStructure.h"

namespace HBE {
	class VK_Device;

	class VK_TopLevelAccelerationStructure : public RootAccelerationStructure {
		VK_Device *device;
	public:
		VK_TopLevelAccelerationStructure(VK_Device *device, RootAccelerationStructureInfo info);
	};


}