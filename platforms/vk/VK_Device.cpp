
#include "VK_Device.h"
#include "core/utility/Log.h"
#include "vector"
#include "set"
#include "VK_Allocator.h"

namespace HBE {
	VK_Device::VK_Device(VK_PhysicalDevice &physical_device) {
		this->physical_device = &physical_device;
		QueueFamilyIndices indices = physical_device.getQueueFamilyIndices();

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		std::set<uint32_t> uniqueQueueFamilies;
		uniqueQueueFamilies.emplace(indices.graphics_family.value());
		uniqueQueueFamilies.emplace(indices.present_family.value());
		if (indices.compute_family.has_value())uniqueQueueFamilies.emplace(indices.compute_family.value());
		if (indices.transfer_family.has_value())uniqueQueueFamilies.emplace(indices.transfer_family.value());

		float priority = 1.0f;//between 0 and 1.
		for (uint32_t queue_family: uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queue_create_info{};
			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = queue_family;
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &priority;
			queue_create_infos.push_back(queue_create_info);
		}

		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		device_create_info.pQueueCreateInfos = queue_create_infos.data();
		VkPhysicalDeviceFeatures device_features{};
		device_features.samplerAnisotropy = physical_device.getFeatures().samplerAnisotropy;
		device_create_info.pEnabledFeatures = &device_features;
		auto &enabled_extensions = physical_device.getExtensions();
		device_create_info.enabledExtensionCount = enabled_extensions.size();
		device_create_info.ppEnabledExtensionNames = enabled_extensions.data();



		VkPhysicalDeviceRobustness2FeaturesEXT robustness_features{};
		robustness_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
		robustness_features.nullDescriptor = true;


		EXTENSION_FLAGS enabled_extensions_flags = physical_device.getEnabledExtensionFlags();

		VkPhysicalDeviceBufferDeviceAddressFeatures buffer_device_address_features{};
		buffer_device_address_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
		buffer_device_address_features.bufferDeviceAddress = (enabled_extensions_flags & EXTENSION_FLAG_BUFFER_DEVICE_ADDRESS)>0;

		VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray_tracing_features{};
		ray_tracing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
		ray_tracing_features.rayTracingPipeline = (enabled_extensions_flags & EXTENSION_FLAG_RAY_TRACING_PIPELINE)>0;

		VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_features{};
		acceleration_structure_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
		acceleration_structure_features.accelerationStructure = (enabled_extensions_flags & EXTENSION_FLAG_ACCELERATION_STRUCTURE)>0;

		VkPhysicalDeviceDescriptorIndexingFeatures descriptor_indexing_features{};
		descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
		descriptor_indexing_features.runtimeDescriptorArray = physical_device.getDescriptorIndexingFeatures().runtimeDescriptorArray;
		descriptor_indexing_features.descriptorBindingPartiallyBound = physical_device.getDescriptorIndexingFeatures().descriptorBindingPartiallyBound;
		descriptor_indexing_features.descriptorBindingVariableDescriptorCount = physical_device.getDescriptorIndexingFeatures().descriptorBindingVariableDescriptorCount;
		descriptor_indexing_features.shaderSampledImageArrayNonUniformIndexing = physical_device.getDescriptorIndexingFeatures().shaderSampledImageArrayNonUniformIndexing;
		descriptor_indexing_features.shaderStorageBufferArrayNonUniformIndexing = physical_device.getDescriptorIndexingFeatures().shaderStorageBufferArrayNonUniformIndexing;
		descriptor_indexing_features.shaderStorageImageArrayNonUniformIndexing = physical_device.getDescriptorIndexingFeatures().shaderStorageImageArrayNonUniformIndexing;
		descriptor_indexing_features.shaderUniformBufferArrayNonUniformIndexing = physical_device.getDescriptorIndexingFeatures().shaderUniformBufferArrayNonUniformIndexing;
		descriptor_indexing_features.shaderUniformTexelBufferArrayNonUniformIndexing = physical_device.getDescriptorIndexingFeatures().shaderUniformTexelBufferArrayNonUniformIndexing;
		descriptor_indexing_features.shaderStorageTexelBufferArrayNonUniformIndexing = physical_device.getDescriptorIndexingFeatures().shaderStorageTexelBufferArrayNonUniformIndexing;
		descriptor_indexing_features.descriptorBindingUpdateUnusedWhilePending = physical_device.getDescriptorIndexingFeatures().descriptorBindingUpdateUnusedWhilePending;
		descriptor_indexing_features.descriptorBindingPartiallyBound = physical_device.getDescriptorIndexingFeatures().descriptorBindingPartiallyBound;
		descriptor_indexing_features.descriptorBindingVariableDescriptorCount = physical_device.getDescriptorIndexingFeatures().descriptorBindingVariableDescriptorCount;



		device_create_info.pNext=&robustness_features;
		void** ppNext=&robustness_features.pNext;
		if(enabled_extensions_flags & EXTENSION_FLAG_BUFFER_DEVICE_ADDRESS)
		{
			*ppNext = &buffer_device_address_features;
			ppNext = &buffer_device_address_features.pNext;
		}
		if(enabled_extensions_flags & EXTENSION_FLAG_RAY_TRACING_PIPELINE)
		{
			*ppNext = &ray_tracing_features;
			ppNext = &ray_tracing_features.pNext;
		}
		if(enabled_extensions_flags & EXTENSION_FLAG_ACCELERATION_STRUCTURE)
		{
			*ppNext = &acceleration_structure_features;
			ppNext = &acceleration_structure_features.pNext;
		}
		if(enabled_extensions_flags & EXTENSION_FLAG_DESCRIPTOR_INDEXING)
		{
			*ppNext = &descriptor_indexing_features;
			ppNext = &descriptor_indexing_features.pNext;
		}

		if (vkCreateDevice(physical_device.getHandle(), &device_create_info, nullptr, &handle) != VK_SUCCESS) {
			Log::error("Failed to create logical device");
		}
		queues.try_emplace(QUEUE_FAMILY_GRAPHICS, this, QUEUE_FAMILY_GRAPHICS, indices.graphics_family.value());
		queues.try_emplace(QUEUE_FAMILY_PRESENT, this, QUEUE_FAMILY_PRESENT, indices.present_family.value());
		if (indices.compute_family.has_value())
			queues.try_emplace(QUEUE_FAMILY_COMPUTE, this, QUEUE_FAMILY_COMPUTE, indices.compute_family.value());
		if (indices.transfer_family.has_value())
			queues.try_emplace(QUEUE_FAMILY_TRANSFER, this, QUEUE_FAMILY_TRANSFER, indices.transfer_family.value());
		allocator = new VK_Allocator(this);
		initFunctionPointers();
	}

	VK_Device::~VK_Device() {
		delete allocator;
		queues.clear();
		vkDestroyDevice(handle, nullptr);
	}

	const VkDevice &VK_Device::getHandle() const {
		return handle;
	}


	const VK_PhysicalDevice &VK_Device::getPhysicalDevice() const {
		return *physical_device;
	}

	void VK_Device::wait() {
		vkDeviceWaitIdle(handle);
	}


	VK_Allocator *VK_Device::getAllocator() {
		return allocator;
	}

	VK_Queue &VK_Device::getQueue(QUEUE_FAMILY family) {
		HB_ASSERT(queues.contains(family), "Queue family " + std::to_string(family) + " not found");
		return queues.at(family);
	}

	void VK_Device::initFunctionPointers() {
		vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(handle, "vkGetBufferDeviceAddressKHR"));
		vkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(handle, "vkCmdBuildAccelerationStructuresKHR"));
		vkBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(handle, "vkBuildAccelerationStructuresKHR"));
		vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(handle, "vkCreateAccelerationStructureKHR"));
		vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(handle, "vkDestroyAccelerationStructureKHR"));
		vkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(handle, "vkGetAccelerationStructureBuildSizesKHR"));
		vkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(handle, "vkGetAccelerationStructureDeviceAddressKHR"));
		vkCmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(handle, "vkCmdTraceRaysKHR"));
		vkGetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(handle, "vkGetRayTracingShaderGroupHandlesKHR"));
		vkCreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(handle, "vkCreateRayTracingPipelinesKHR"));
	}

	bool VK_Device::hasQueue(QUEUE_FAMILY family) {
		return queues.contains(family);
	}
}