#include "VK_Device.h"
#include "VK_BottomLevelAccelerationStructure.h"
#include "VK_Buffer.h"
namespace HBE {
	VK_BottomLevelAccelerationStructure::VK_BottomLevelAccelerationStructure(VK_Device &device) : device(device) {

		VkAccelerationStructureCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		create_info.pNext = nullptr;
		create_info.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;


		VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info{};

		acceleration_structure_create_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		acceleration_structure_create_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		acceleration_structure_create_info.createFlags = VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;



		acceleration_structure_create_info.offset = 0;
		device.vkCreateAccelerationStructureKHR(device.getHandle(), &acceleration_structure_create_info, nullptr, &handle);

		//vkCreateAccelerationStructureKHR(device.getHandle(), &acceleration_structure_create_info, nullptr, &handle);

		/*VkAccelerationStructureBuildGeometryInfoKHR acceleration_structure_build_geometry_info{};
		acceleration_structure_build_geometry_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		acceleration_structure_build_geometry_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		acceleration_structure_build_geometry_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
		acceleration_structure_build_geometry_info.pGeometries = nullptr;
		acceleration_structure_build_geometry_info.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		acceleration_structure_build_geometry_info.dstAccelerationStructure = handle;

		VkAccelerationStructureBuildRangeInfoKHR acceleration_structure_build_range_info{};
		acceleration_structure_build_range_info.primitiveCount = 0;
		acceleration_structure_build_range_info.primitiveOffset = 0;
		acceleration_structure_build_range_info.firstVertex = 0;
		acceleration_structure_build_range_info.transformOffset = 0;

		const VkAccelerationStructureBuildRangeInfoKHR* info_ptr=&acceleration_structure_build_range_info;
		//GPU SIDE
		//vkCmdBuildAccelerationStructuresKHR(command_buffer,device.getHandle(), nullptr, 1, &acceleration_structure_build_geometry_info, &info_ptr);
		//CPU SIDE:
		//vkBuildAccelerationStructuresKHR(device.getHandle(), nullptr, 1, &acceleration_structure_build_geometry_info, &info_ptr);*/
	}

	VK_BottomLevelAccelerationStructure::~VK_BottomLevelAccelerationStructure() {
		device.vkDestroyAccelerationStructureKHR(device.getHandle(), handle, nullptr);
	}

	VkAccelerationStructureKHR VK_BottomLevelAccelerationStructure::getHandle() const {
		return handle;
	}


}

