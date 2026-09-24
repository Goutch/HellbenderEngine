//
// Created by user on 4/17/2021.
//

#include "VK_Surface.h"

#include "VK_Instance.h"
#include "core/utility/Log.h"
#include "GLFW/glfw3.h"

namespace HBE
{
    VkSurfaceKHR& VK_Surface::getHandle()
    {
        return handle;
    }

    void VK_Surface::init(VK_Instance& instance, GLFWwindow* window_handle)
    {
        this->instance_handle = instance.getHandle();
        if (glfwCreateWindowSurface(instance_handle, window_handle, nullptr, &handle) != VK_SUCCESS)
        {
            Log::error("Failed to create window surface");
        }
    }

    void VK_Surface::release()
    {
        vkDestroySurfaceKHR(instance_handle, handle, nullptr);
    }
}
