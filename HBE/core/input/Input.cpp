

#include <core/utility/Log.h>
#include "Input.h"
#include "GLFW/glfw3.h"
#include "core/graphics/Graphics.h"
#include "core/graphics/Window.h"

namespace HBE {
    GLFWwindow *Input::window = nullptr;
    float Input::wheel_offset = 0.0f;

    bool Input::getKey(KEY code) {
        return glfwGetKey(window, code) == GLFW_PRESS;
    }

    void Input::getMousePosition(double &x, double &y) {
        glfwGetCursorPos(window, &x, &y);
    }

    void Input::init() {
        Input::window = static_cast<GLFWwindow *>(Graphics::getWindow()->getHandle());
        glfwSetScrollCallback(window, scrollCallback);
    }

    void Input::getMouseWheelInput(float &value) {
        value = wheel_offset;
    }

    void Input::setCursorVisible(bool visible) {
        visible ? glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL) : glfwSetInputMode(window, GLFW_CURSOR,
                                                                                               GLFW_CURSOR_HIDDEN);
    }

    void Input::setCursorPosition(double x, double y) {
        glfwSetCursorPos(window, x, y);
    }

    void Input::scrollCallback(GLFWwindow *window, double x_offset, double y_offset) {
        wheel_offset = y_offset;
    }

    void Input::pollEvents() {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_KEY_DOWN) {
            Log::message("patate3");
        }
    }
}




