#include "VK_Window.h"
#include "GLFW/glfw3.h"
#include "core/utility/Log.h"
#include "core/Configs.h"
#include "VK_RenderPass.h"
#include "core/Application.h"
#include "stb_image.h"
#include "stdio.h"

namespace HBE
{
    void VK_Window::windowSizeCallback(GLFWwindow* handle, int width, int height)
    {
        Window* window = (Window*)glfwGetWindowUserPointer(handle);
        VK_Window* instance = reinterpret_cast<VK_Window*>(glfwGetWindowUserPointer(handle));
        Log::status("Window size changed from (" + std::to_string(instance->last_width) + " x " + std::to_string(instance->last_height) + ") to (" + std::to_string(width) + " x " +
            std::to_string(height) + ")");
        instance->last_width = instance->width;
        instance->last_height = instance->height;
        instance->width = width;
        instance->height = height;
        if (width == 0 || height == 0)
        {
            instance->minimized = true;
        }
        else
        {
            instance->minimized = false;
        }
        window->onSizeChange.invoke(window);
    }

    void VK_Window::onTitleChange(std::string title)
    {
        glfwSetWindowTitle(handle, title.c_str());
    }

    VK_Window::VK_Window(WindowInfo& info)
    {
        this->width = info.startSize.x;
        this->height = info.startSize.y;
        if (!glfwInit())
        {
            Log::error("Failed to load glfw");
        }
        if (!glfwVulkanSupported())
        {
            Log::error("Vulkan is not supported");
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        handle = glfwCreateWindow(width, height, Application::instance->getInfo().app_name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(handle, (void*)this);
        glfwSetWindowSizeCallback(handle, windowSizeCallback);

        setLogo(Configs::getWindowIconPaths());
    }

    VK_Window::~VK_Window()
    {
        glfwTerminate();
    }

    bool VK_Window::shouldClose()
    {
        return glfwWindowShouldClose(handle);
    }
    void VK_Window::requestClose()
    {
        glfwSetWindowShouldClose(handle, true);
    }

    void VK_Window::getSize(uint32_t& width, uint32_t& height)
    {
        width = this->width;
        height = this->height;
    }

    GLFWwindow* VK_Window::getHandle()
    {
        return handle;
    }

    void VK_Window::setFullscreen(bool fullscreen)
    {
        if (is_fullscreen != fullscreen)
        {
            if (fullscreen)
            {
                last_width = width;
                last_height = height;
                // backup window position and window size
                glfwGetWindowPos(handle, &last_x, &last_y);

                // get resolution of monitor
                const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

                // switch to full screen
                glfwSetWindowMonitor(handle, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
                width = mode->width;
                height = mode->height;
            }
            else
            {
                // restore last window size and position
                int temp_width = width;
                int temp_height = height;
                width = last_width;
                height = last_height;
                last_width = temp_width;
                last_height = temp_height;

                glfwSetWindowMonitor(handle, nullptr, last_x, last_y, width, height, GLFW_DONT_CARE);
            }
            is_fullscreen = fullscreen;
        }
    }

    bool VK_Window::isMinimized()
    {
        return minimized;
    }

    uint32_t VK_Window::getWidth()
    {
        return width;
    }

    uint32_t VK_Window::getHeight()
    {
        return height;
    }


    void VK_Window::setLogo(const std::vector<std::string>& paths)
    {
        stbi_set_flip_vertically_on_load(false);
        std::vector<GLFWimage> images;
        for (auto path : paths)
        {
            FILE* file = fopen((RESOURCE_PATH + std::string(path)).c_str(), "rb");
            if (file == nullptr)
            {
                Log::error("Failed to open file: " + std::string(path));
            }
            int32_t image_width, image_height;
            unsigned char* buffer;
            int nb_channels;
            int expected_channels = 4;
            buffer = stbi_load_from_file(file, &image_width, &image_height, &nb_channels, expected_channels);

            if (nb_channels != expected_channels)
            {
                Log::error("Texture format missmatch. Expected " + std::to_string(expected_channels) + " channels, got " + std::to_string(nb_channels));
            }
            if (buffer == nullptr)
            {
                Log::error("Failed to load texture: " + std::string(path));
            }
            images.push_back({image_width, image_height, buffer});
            fclose(file);
        }


        glfwSetWindowIcon(handle, images.size(), images.data());


        for (auto image : images)
        {
            stbi_image_free(image.pixels);
        }
    }
}
