#pragma once
#include "vulkan/vulkan.h"
struct GLFWwindow;
namespace HBE
{
    class VK_Surface {
        VkSurfaceKHR handle;
       const VkInstance* instance_handle;
    public:
        VK_Surface(const VkInstance &instance_handle,GLFWwindow* window_handle);
        ~VK_Surface();
        VkSurfaceKHR &getHandle();
    };

}
