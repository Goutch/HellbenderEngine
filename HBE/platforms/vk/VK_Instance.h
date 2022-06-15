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
		VK_ValidationLayers *validation_layers = nullptr;


	public:
		VK_Instance();

		~VK_Instance();

		VkInstance getHandle() const;
	private:
		void getRequiredExtensions(std::vector<const char *> &required_extensions);

		bool checkExtensionsSupported(std::vector<const char *> &required_extensions);
	};
}