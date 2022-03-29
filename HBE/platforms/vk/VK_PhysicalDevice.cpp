
#include <vector>
#include "VK_PhysicalDevice.h"
#include "core/utility/Log.h"
#include "map"
#include <cstring>

namespace HBE {
	VK_PhysicalDevice::VK_PhysicalDevice(VkInstance vk_instance_handle, VkSurfaceKHR surface_handle) {
        this->vk_instance_handle = vk_instance_handle;
        this->surface_handle = surface_handle;
        Log::status("Looking for suitable GPU:");
        pickBestPhysicalDevice();

        support_details = querySwapchainSupportDetails(handle);
		vkGetPhysicalDeviceMemoryProperties(handle,&memory_properties);
		vkGetPhysicalDeviceFeatures(handle, &supported_features);
    }

    void VK_PhysicalDevice::pickBestPhysicalDevice() {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(vk_instance_handle, &device_count, nullptr);
        if (device_count == 0) {
            Log::error("No available GPU!");
        }
        std::vector<VkPhysicalDevice> available_physical_devices = std::vector<VkPhysicalDevice>(device_count);
        vkEnumeratePhysicalDevices(vk_instance_handle, &device_count, available_physical_devices.data());

        std::multimap<int, VkPhysicalDevice> suitable_devices_map;
        for (const VkPhysicalDevice &physical_device:available_physical_devices) {
            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(physical_device, &device_properties);
            Log::message(std::string("\t") + device_properties.deviceName + ":");
            if (isDeviceSuitable(physical_device)) {
                int score = getScore(physical_device);
                suitable_devices_map.emplace(score, physical_device);
            }
        }
        if (!suitable_devices_map.empty()) {
            handle = suitable_devices_map.rbegin()->second;
            vkGetPhysicalDeviceProperties(handle, &properties);

            queue_family_indices = getSupportedQueueFamilies(handle);
            Log::status(std::string("\tFOUND:") + properties.deviceName);
        } else {
            Log::error("\tFailed to find a suitable GPU!");
        }

    }

    int VK_PhysicalDevice::getScore(const VkPhysicalDevice &physical_device) {
        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceProperties(physical_device, &device_properties);
        vkGetPhysicalDeviceFeatures(physical_device, &device_features);
        int score = 1;
        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }
        return score;
    }

    const QueueFamilyIndices &VK_PhysicalDevice::getQueueFamilyIndices() const {
        return queue_family_indices;
    }

    bool VK_PhysicalDevice::isDeviceSuitable(VkPhysicalDevice const &physical_device) {

        QueueFamilyIndices queue_families_indices = getSupportedQueueFamilies(physical_device);
        bool swapchain_support = false;
        bool extension_support = checkExtensionsSupport(physical_device);
        if (extension_support) {
            SwapchainSupportDetails swapchain_support_details = querySwapchainSupportDetails(physical_device);
            swapchain_support =
                    !swapchain_support_details.present_modes.empty() && !swapchain_support_details.formats.empty();
        }

        return queue_families_indices.isComplete() && extension_support && swapchain_support;
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
                //tranfer
            else if ((supported_queue_families.at(i).queueFlags & VK_QUEUE_TRANSFER_BIT)) {
                indices.transfer_family = i;
            }
            //complete?
            if (indices.isComplete())break;
        }

        return indices;
    }


    const VkPhysicalDeviceMemoryProperties& VK_PhysicalDevice::getMemoryProperties() const
	{
		return memory_properties;
	}
    const VkPhysicalDevice &VK_PhysicalDevice::getHandle() const {
        return handle;
    }

    const VkPhysicalDeviceFeatures &VK_PhysicalDevice::getFeatures() const {
        return supported_features;
    }

    const VkPhysicalDeviceProperties &VK_PhysicalDevice::getProperties() const {
        return properties;
    }

    bool VK_PhysicalDevice::checkExtensionsSupport(VkPhysicalDevice const &physical_device) {
        uint32_t extention_count = 0;
        vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extention_count, nullptr);
        std::vector<VkExtensionProperties> available_extensions(extention_count);
        vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extention_count, available_extensions.data());
        Log::message("\t\tLooking for required device extensions:");
        for (const char *required_extension:device_required_extensions) {
            Log::message(std::string("\t\t\t") + required_extension + "...");
            bool found = false;
            for (auto &available_extension:available_extensions) {
                if (strcmp(required_extension, available_extension.extensionName) == 0) {
                    found = true;
                    Log::message("\t\t\t\tFOUND!");
                    break;
                }
            }
            if (!found) {
                Log::message("\t\t\t\tMISSING!");
                return false;
            }
        }

        return true;

    }

    const std::vector<const char *> &VK_PhysicalDevice::getRequiredExtensions() const {
        return device_required_extensions;
    }

    SwapchainSupportDetails VK_PhysicalDevice::querySwapchainSupportDetails(VkPhysicalDevice const &physical_device) const {
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



}




