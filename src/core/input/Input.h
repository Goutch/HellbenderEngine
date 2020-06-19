#pragma once

class GLFWwindow;
class Input {
    static GLFWwindow* window;
public:
    static void init(GLFWwindow* window);
    static void getMousePosition(double &x, double &y);
    static bool getInput(int code);
    static void getMouseWheelInput(float& value);
    static void setCursorVisible(bool visible);
    static void setCursorPosition(double x,double y);
};


