#pragma once

#include "vulkan/vulkan.h"

struct GLFWwindow;
namespace HBE {
	class VK_Surface {
		VkSurfaceKHR handle;
		VkInstance instance_handle;
	public:
		VK_Surface(VkInstance instance_handle, GLFWwindow *window_handle);
		~VK_Surface();
		VkSurfaceKHR &getHandle();
	};

}
