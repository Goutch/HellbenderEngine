

#include "Input.h"
#include "GLFW/glfw3.h"

GLFWwindow *Input::window = nullptr;
float Input::wheel_offset =0.0f;
bool Input::getKeyDown(KEY code) {
    return glfwGetKey(window, code) == GLFW_PRESS;
}

void Input::getMousePosition(double &x, double &y) {
    glfwGetCursorPos(window, &x, &y);
}

void Input::init(GLFWwindow *window) {
    Input::window = window;
    glfwSetScrollCallback(window,scrollCallback);
}

void Input::getMouseWheelInput(float &value) {
    value=wheel_offset;
}

void Input::setCursorVisible(bool visible) {
    visible ? glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL) : glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Input::setCursorPosition(double x, double y) {
    glfwSetCursorPos(window, x, y);
}

void Input::scrollCallback(GLFWwindow *window, double x_offset, double y_offset) {
    wheel_offset=y_offset;
}

