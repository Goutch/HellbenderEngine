#include <vector>
#include "VK_PhysicalDevice.h"
#include "core/utility/Log.h"
#include "map"
#include <cstring>
#include "Application.h"

namespace HBE {
	VK_PhysicalDevice::VK_PhysicalDevice(VkInstance vk_instance_handle, VkSurfaceKHR surface_handle) {
		this->vk_instance_handle = vk_instance_handle;
		this->surface_handle = surface_handle;
		Log::status("Looking for suitable GPU...");
		pickBestPhysicalDevice();

		support_details = querySwapchainSupportDetails(handle);
		vkGetPhysicalDeviceMemoryProperties(handle, &memory_properties);

		ApplicationInfo app_info = Application::getInfo();
		features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

		void **ppNext = &features2.pNext;
		if (true) {
			robustness2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
			enabled_extension_flags |= EXTENSION_FLAG_NULL_DESCRIPTORS;
			*ppNext = &robustness2_features;
			ppNext = &robustness2_features.pNext;
		}
		if (app_info.required_extension_flags & VULKAN_REQUIRED_EXTENSION_RTX) {
			enabled_extension_flags |= EXTENSION_FLAG_RAY_TRACING_PIPELINE;
			enabled_extension_flags |= EXTENSION_FLAG_ACCELERATION_STRUCTURE;
			enabled_extension_flags |= EXTENSION_FLAG_BUFFER_DEVICE_ADDRESS;

			buffer_device_address_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
			acceleration_structure_features.sType =
					VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
			ray_tracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
			*ppNext = &buffer_device_address_features;
			ppNext = &buffer_device_address_features.pNext;

			buffer_device_address_features.pNext = &acceleration_structure_features;
			acceleration_structure_features.pNext = &ray_tracing_pipeline_features;
			*ppNext = &ray_tracing_pipeline_features;
			ppNext = &ray_tracing_pipeline_features.pNext;
		}
		if (app_info.required_extension_flags & VULKAN_REQUIRED_EXTENSION_DESCRIPTOR_INDEXING ||
		    app_info.required_extension_flags & VULKAN_REQUIRED_EXTENSION_RTX) {
			enabled_extension_flags |= EXTENSION_FLAG_DESCRIPTOR_INDEXING;
			descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
			*ppNext = &descriptor_indexing_features;
			ppNext = &descriptor_indexing_features.pNext;
		}
		if (app_info.vulkan_version >= VULKAN_VERSION_1_3) {
			enabled_extension_flags |= EXTENSION_FLAG_DYNAMIC_RENDERING;
			dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
			*ppNext = &dynamic_rendering_features;
			ppNext = &dynamic_rendering_features.pNext;
		}

		vkGetPhysicalDeviceFeatures2(handle, &features2);
		VkPhysicalDeviceProperties2 properties2{};
		properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		acceleration_structure_properties.sType =
				VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
		ray_tracing_pipeline_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
		descriptor_indexing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;


		properties2.pNext = &acceleration_structure_properties;
		acceleration_structure_properties.pNext = &ray_tracing_pipeline_properties;
		ray_tracing_pipeline_properties.pNext = &descriptor_indexing_properties;
		vkGetPhysicalDeviceProperties2(handle, &properties2);


		features = features2.features;
		properties = properties2.properties;
	}

	void VK_PhysicalDevice::pickBestPhysicalDevice() {
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(vk_instance_handle, &device_count, nullptr);
		if (device_count == 0) {
			Log::error("No available GPU!");
		}
		std::vector<VkPhysicalDevice> available_physical_devices = std::vector<VkPhysicalDevice>(device_count);
		vkEnumeratePhysicalDevices(vk_instance_handle, &device_count, available_physical_devices.data());


		for (const VkPhysicalDevice &physical_device: available_physical_devices) {
			VkPhysicalDeviceProperties device_properties;
			vkGetPhysicalDeviceProperties(physical_device, &device_properties);
			//Log::message(std::string("\t") + device_properties.deviceName + ":");
			if (isDeviceSuitable(physical_device)) {
				calculateScore(physical_device);
			}
		}
		if (!device_score.empty()) {
			handle = device_score.rbegin()->second;
			vkGetPhysicalDeviceProperties(handle, &properties);
			vkGetPhysicalDeviceFeatures(handle, &features);
			queue_family_indices = getSupportedQueueFamilies(handle);
			enabled_extensions = device_enabled_extensions[handle];

			Log::message(std::string("\t\t\tFound : ") + properties.deviceName);
		} else {
			Log::error("\tFailed to find a suitable GPU!");
		}
	}

	void VK_PhysicalDevice::calculateScore(const VkPhysicalDevice &physical_device) {
		VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures device_features;
		vkGetPhysicalDeviceProperties(physical_device, &device_properties);
		vkGetPhysicalDeviceFeatures(physical_device, &device_features);
		uint32_t score = 1;
		if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}
		score += static_cast<uint32_t>(device_enabled_extensions[physical_device].size());

		device_score.emplace(score, physical_device);
	}

	const QueueFamilyIndices &VK_PhysicalDevice::getQueueFamilyIndices() const {
		return queue_family_indices;
	}

	bool VK_PhysicalDevice::isDeviceSuitable(VkPhysicalDevice const &physical_device) {
		QueueFamilyIndices queue_families_indices = getSupportedQueueFamilies(physical_device);
		if (!queue_families_indices.hasMinimumRequirements())
			return false;

		if (!checkExtensionsSupport(physical_device, REQUIRED_EXTENSIONS))
			return false;

		SwapchainSupportDetails swapchain_support_details = querySwapchainSupportDetails(physical_device);
		if (swapchain_support_details.present_modes.empty() || swapchain_support_details.formats.empty())
			return false;

		device_enabled_extensions.emplace(physical_device, REQUIRED_EXTENSIONS);


		ApplicationInfo app_info = Application::getInfo();

		{
			if (!checkExtensionsSupport(physical_device, ROBUSTNESS_EXTENSIONS)) {
				return false;
			}
			device_enabled_extensions[physical_device].insert(device_enabled_extensions[physical_device].end(),
			                                                  ROBUSTNESS_EXTENSIONS.begin(),
			                                                  ROBUSTNESS_EXTENSIONS.end());
		}

		if (app_info.required_extension_flags & VULKAN_REQUIRED_EXTENSION_RTX) {
			const std::vector<const char *> *raytracing_extensions;
			if (app_info.vulkan_version >= VULKAN_VERSION_1_2)
				raytracing_extensions = &RAYTRACING_EXTENSIONS_1_2;
			else
				raytracing_extensions = &RAYTRACING_EXTENSIONS_1_1;

			if (!checkExtensionsSupport(physical_device, *raytracing_extensions))
				return false;
			device_enabled_extensions[physical_device].insert(device_enabled_extensions[physical_device].end(),
			                                                  raytracing_extensions->begin(),
			                                                  raytracing_extensions->end());
		}

		//descriptor indexing added to Vulkan 1.2
		if ((app_info.required_extension_flags & VULKAN_REQUIRED_EXTENSION_DESCRIPTOR_INDEXING) &&
		    (app_info.vulkan_version < VULKAN_VERSION_1_2)) {
			if (!checkExtensionsSupport(physical_device, DESCRIPTOR_INDEXING_EXTENSIONS_1_1))
				return false;
			device_enabled_extensions[physical_device].insert(device_enabled_extensions[physical_device].end(),
			                                                  DESCRIPTOR_INDEXING_EXTENSIONS_1_1.begin(),
			                                                  DESCRIPTOR_INDEXING_EXTENSIONS_1_1.end());
		}

		if (checkExtensionsSupport(physical_device, DEBUG_EXTENSIONS))
			device_enabled_extensions[physical_device].insert(device_enabled_extensions[physical_device].end(),
			                                                  DEBUG_EXTENSIONS.begin(), DEBUG_EXTENSIONS.end());


		return true;
	}

	QueueFamilyIndices VK_PhysicalDevice::getSupportedQueueFamilies(VkPhysicalDevice const &physical_device) {
		QueueFamilyIndices indices;
		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
		std::vector<VkQueueFamilyProperties> supported_queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, supported_queue_families.data());


		for (uint32_t i = 0; i < queue_family_count; ++i) {
			//present
			VkBool32 present_supported = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface_handle, &present_supported);
			if (present_supported) {
				indices.present_family = i;
			}
			//graphics
			if (supported_queue_families.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphics_family = i;
			}
				//compute
			else if (supported_queue_families.at(i).queueFlags & VK_QUEUE_COMPUTE_BIT) {
				indices.compute_family = i;
			}
				//transfer
			else if ((supported_queue_families.at(i).queueFlags & VK_QUEUE_TRANSFER_BIT)) {
				indices.transfer_family = i;
			}

			//complete?
			if (indices.isComplete())break;
		}


		return indices;
	}


	const VkPhysicalDevice &VK_PhysicalDevice::getHandle() const {
		return handle;
	}


	bool VK_PhysicalDevice::checkExtensionsSupport(VkPhysicalDevice const &physical_device,
	                                               const std::vector<const char *> &extensions) {
		uint32_t extention_count = 0;
		vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extention_count, nullptr);
		std::vector<VkExtensionProperties> available_extensions(extention_count);
		vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extention_count, available_extensions.data());
		std::vector<bool> found;
		found.resize(extensions.size(), false);

		for (auto &available_extension: available_extensions) {
			for (int i = 0; i < extensions.size(); ++i) {
				if (strcmp(extensions[i], available_extension.extensionName) == 0) {
					found[i] = true;
					break;
				}
			}
		}

		for (int i = 0; i < found.size(); ++i) {
			if (!found[i])
				return false;
		}
		return true;
	}


	const std::vector<const char *> &VK_PhysicalDevice::getExtensions() const {
		return enabled_extensions;
	}

	SwapchainSupportDetails VK_PhysicalDevice::querySwapchainSupportDetails(
			VkPhysicalDevice const &physical_device) const {
		SwapchainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface_handle, &details.capabilities);

		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
		                                     surface_handle,
		                                     &format_count,
		                                     nullptr);

		if (format_count != 0) {
			details.formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
			                                     surface_handle,
			                                     &format_count,
			                                     details.formats.data());
		}

		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
		                                          surface_handle,
		                                          &present_mode_count,
		                                          nullptr);

		if (present_mode_count != 0) {
			details.present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
			                                          surface_handle,
			                                          &present_mode_count,
			                                          details.present_modes.data());
		}

		return details;
	}

	const SwapchainSupportDetails &VK_PhysicalDevice::getSwapchainSupportDetails() const {
		return support_details;
	}

	const VkPhysicalDeviceAccelerationStructureFeaturesKHR &VK_PhysicalDevice::getAccelerationStructureFeatures() const {
		return acceleration_structure_features;
	}

	const VkPhysicalDeviceRayTracingPipelineFeaturesKHR &VK_PhysicalDevice::getRayTracingPipelineFeatures() const {
		return ray_tracing_pipeline_features;
	}

	const VkPhysicalDeviceFeatures &VK_PhysicalDevice::getFeatures() const {
		return features;
	}

	const VkPhysicalDeviceMemoryProperties &VK_PhysicalDevice::getMemoryProperties() const {
		return memory_properties;
	}

	const VkPhysicalDeviceProperties &VK_PhysicalDevice::getProperties() const {
		return properties;
	}

	const VkFormatProperties &VK_PhysicalDevice::getFormatProperties() const {
		return format_properties;
	}

	const VkPhysicalDeviceRayTracingPipelinePropertiesKHR &VK_PhysicalDevice::getRayTracingPipelineProperties() const {
		return ray_tracing_pipeline_properties;
	}

	const VkPhysicalDeviceRobustness2FeaturesEXT VK_PhysicalDevice::getRobustnessFeatures() {
		return robustness2_features;
	}

	const VkPhysicalDeviceAccelerationStructurePropertiesKHR &
	VK_PhysicalDevice::getAccelerationStructureProperties() const {
		return acceleration_structure_properties;
	}

	const VkPhysicalDeviceBufferDeviceAddressFeatures &VK_PhysicalDevice::getBufferDeviceAddressFeatures() const {
		return buffer_device_address_features;
	}

	const VkPhysicalDeviceDescriptorIndexingFeatures &VK_PhysicalDevice::getDescriptorIndexingFeatures() const {
		return descriptor_indexing_features;
	}

	const EXTENSION_FLAGS &VK_PhysicalDevice::getEnabledExtensionFlags() const {
		return enabled_extension_flags;
	}

	const VkPhysicalDeviceFeatures2& VK_PhysicalDevice::getFeatures2() {
		return features2;
	}
}
