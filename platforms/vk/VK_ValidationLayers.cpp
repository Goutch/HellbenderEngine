//
// Created by user on 2021-02-19.
//

#include <cstring>
#include "VK_ValidationLayers.h"
#include "vulkan/vulkan.h"

namespace HBE {

	bool VK_ValidationLayers::checkValidationLayerSupport() {
		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

		std::vector<VkLayerProperties> availableLayers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, availableLayers.data());

		for (const char *layerName: validation_layer_names) {
			bool layerFound = false;

			for (const auto &layerProperties: availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	VK_ValidationLayers::VK_ValidationLayers() {
		create_info = {};
		create_info.sType =
				VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		create_info.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = debugCallback;

	}

	void VK_ValidationLayers::init(const VkInstance &instance_handle) {
		this->instance_handle = &instance_handle;

		if (createDebugUtilsMessengerEXT(instance_handle, &create_info, nullptr, &debugMessenger) != VK_SUCCESS) {
			Log::warning("failed to set up debug messenger!");
		}

	}

	VkBool32 VK_ValidationLayers::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
												VkDebugUtilsMessageTypeFlagsEXT messageType,
												const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
												void *pUserData) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			Log::error(pCallbackData->pMessage);
		} else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			Log::warning(pCallbackData->pMessage);
		}
		else if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		{
			Log::debug(pCallbackData->pMessage);
		}



		return VK_FALSE;
	}

	VK_ValidationLayers::~VK_ValidationLayers() {
		destroyDebugUtilsMessengerEXT();
	}

	VkResult VK_ValidationLayers::createDebugUtilsMessengerEXT(VkInstance instance,
															   const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
															   const VkAllocationCallbacks *pAllocator,
															   VkDebugUtilsMessengerEXT *pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
																			   "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		} else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VK_ValidationLayers::destroyDebugUtilsMessengerEXT() {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(*instance_handle,
																				"vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(*instance_handle, debugMessenger, nullptr);
		}
	}

	VkDebugUtilsMessengerCreateInfoEXT &VK_ValidationLayers::getCreateInfo() {
		return create_info;
	}


}
