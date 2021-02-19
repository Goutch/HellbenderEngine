
#include "GL_Window.h"
#include "GLFW/glfw3.h"
#include "core/utility/Log.h"
#include "Configs.h"

namespace HBE {
    void GL_Window::windowSizeCallback(GLFWwindow *handle, int width, int height) {
        Window *window = (Window *) glfwGetWindowUserPointer(handle);
        window->onWindowSizeChange.invoke(width, height);
    }

    GL_Window::GL_Window(int width, int height) {
        if (!glfwInit()) {
            Log::error("Failed to load glfw");
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        handle = glfwCreateWindow(width, height, Configs::getWindowTitle().c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(handle);
        glfwSetWindowUserPointer(handle, (void *) this);
        glfwSetWindowSizeCallback(handle, windowSizeCallback);

        if (!Configs::getVerticalSync())
            onVerticalSyncChange(0);
        onTitleChange(Configs::getWindowTitle());
        Configs::onWindowTitleChange.subscribe(this, &GL_Window::onTitleChange);
        Configs::onVerticalSyncChange.subscribe(this, &GL_Window::onVerticalSyncChange);
    }

    void GL_Window::onVerticalSyncChange(bool v_sync) {
        glfwSwapInterval(v_sync);
    }

    GL_Window::~GL_Window() {
        Configs::onVerticalSyncChange.unsubscribe(this);
        Configs::onWindowTitleChange.unsubscribe(this);
        glfwTerminate();
    }

    bool GL_Window::shouldClose() {
        return glfwWindowShouldClose(handle);
    }

    void GL_Window::swapBuffers() {
        glfwSwapBuffers(handle);
    }

    void GL_Window::requestClose() {
        glfwSetWindowShouldClose(handle, true);
    }

    void GL_Window::onTitleChange(std::string title) {
        glfwSetWindowTitle(handle, title.c_str());
    }

    void GL_Window::getSize(int &width, int &height) {
        glfwGetWindowSize(handle,&width,&height);
    }

    GLFWwindow *GL_Window::getHandle() {
        return handle;
    }

}
