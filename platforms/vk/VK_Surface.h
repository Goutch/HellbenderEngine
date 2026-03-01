#pragma once

#include "vulkan/vulkan.h"

struct GLFWwindow;

namespace HBE
{
    class VK_Instance;

    class VK_Surface
    {
        VkSurfaceKHR handle;
        VkInstance instance_handle;

    public:
        void init(VK_Instance& instance, GLFWwindow* window_handle);
        void release();
        VK_Surface() = default;
        ~VK_Surface() = default;
        VK_Surface(const VK_Surface&) = delete;
        VK_Surface& operator=(const VK_Surface&) = delete;

        VkSurfaceKHR& getHandle();
    };
}
