#pragma once

#include <vector>
#include "Core.h"
#include "vulkan/vulkan.h"


namespace HBE {
	class VK_ValidationLayers;

	class VK_Instance {
		VkInstance handle;
#ifdef DEBUG_MODE
        const bool ENABLE_VALIDATION_LAYERS = true;
#else
        const bool ENABLE_VALIDATION_LAYERS = false;
#endif
        bool validation_enabled = ENABLE_VALIDATION_LAYERS;
		VK_ValidationLayers *validation_layers = nullptr;
		std::vector<VkValidationFeatureEnableEXT> ENABLED_VALIDATION_FEATURES{
				VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
				//VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
		};
	public:
		VK_Instance();

		~VK_Instance();

		VkInstance getHandle() const;
	private:
		void getRequiredExtensions(std::vector<const char *> &required_extensions);

		bool checkExtensionsSupported(std::vector<const char *> &required_extensions);
	};
}