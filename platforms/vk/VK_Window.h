#pragma once

#include "core/graphics/Window.h"
#include "vulkan/vulkan.h"
#include "string"

struct GLFWwindow;

namespace HBE
{
    class VK_Window : public Window
    {
        GLFWwindow* handle;
        uint32_t width, height;
        uint32_t last_width{}, last_height{};
        int last_x{}, last_y{};
        bool minimized = false;

    public:
        explicit VK_Window(WindowInfo& info);
        ~VK_Window() override;
        uint32_t getWidth() override;
        uint32_t getHeight() override;
        bool shouldClose() override;
        void setFullscreen(bool fullscreen) override;
        void getSize(uint32_t& width, uint32_t& height) override;
        GLFWwindow* getHandle() override;
        void requestClose() override;
        bool isMinimized() override;
        void setLogo(const std::vector<std::string>& paths);
        static void windowSizeCallback(GLFWwindow* handle, int width, int height);
        void onTitleChange(std::string title);

    };
}
