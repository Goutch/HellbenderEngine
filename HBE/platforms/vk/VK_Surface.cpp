//
// Created by user on 4/17/2021.
//

#include "VK_Surface.h"
#include "core/utility/Log.h"
#include "GLFW/glfw3.h"
namespace HBE
{
    VK_Surface::VK_Surface(const VkInstance &instance_handle,GLFWwindow* window_handle) {
        this->instance_handle=&instance_handle;
        if (glfwCreateWindowSurface(instance_handle, window_handle, nullptr, &handle) != VK_SUCCESS) {
            Log::error("Failed to create window surface");
        }
    }

    VkSurfaceKHR &VK_Surface::getHandle() {
        return handle;
    }

    VK_Surface::~VK_Surface() {
        vkDestroySurfaceKHR(*instance_handle,handle, nullptr);
    }
}


