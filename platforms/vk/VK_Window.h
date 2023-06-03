#pragma once

#include "core/graphics/Window.h"
#include "vulkan/vulkan.h"
#include "string"

struct GLFWwindow;
namespace HBE {
	class VK_Window : public Window {
		GLFWwindow *handle;
		uint32_t width, height;
		uint32_t last_width, last_height;
		int last_x, last_y;
		bool minimized = false;
	public:
		VK_Window(const char* title,uint32_t width, uint32_t height);

		~VK_Window();

		uint32_t getWidth() override;

		uint32_t getHeight() override;

		bool shouldClose() override;
		void setFullscreen(bool fullscreen) override;
		void swapBuffers() override;

		void getSize(uint32_t &width, uint32_t &height) override;

		GLFWwindow *getHandle() override;

		void requestClose() override;

		bool isMinimized() override;
		static void windowSizeCallback(GLFWwindow *handle, int width, int height);
		void onTitleChange(std::string title);
	};
}

