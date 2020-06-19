

#include "Input.h"
#include "GLFW/glfw3.h"
GLFWwindow* Input::window= nullptr;
bool Input::getInput(int code) {
    return glfwGetKey(window,code)==GLFW_PRESS;
}

void Input::getMousePosition(double &x, double &y) {
    glfwGetCursorPos(window,&x,&y);
}

void Input::init(GLFWwindow *window) {
    Input::window=window;
}

void Input::getMouseWheelInput(float &value) {

}

void Input::setCursorVisible(bool visible) {
    visible?glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL):glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
}

void Input::setCursorPosition(double x, double y) {
    glfwSetCursorPos(window,x,y);
}

