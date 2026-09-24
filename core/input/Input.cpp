#include <core/utility/Log.h>
#include "Input.h"

#include "core/Application.h"
#include "GLFW/glfw3.h"
#include "core/graphics/Window.h"

namespace HBE
{
    bool Input::getKey(KEY code)
    {
        return pressed[code];
    }

    bool Input::getKeyRepeat(KEY code)
    {
        return repeat[code];
    }

    bool Input::getKeyDown(KEY code)
    {
        return down[code];
    }

    bool Input::getKeyUp(KEY code)
    {
        return released[code];
    }

    vec2 Input::getMousePosition()
    {
        double mouse_x, mouse_y;

        glfwGetCursorPos(window_handle, &mouse_x, &mouse_y);
        //mouse_x / Application::instance->getWindow()->getWidth();
        mouse_y = Application::instance->getWindow()->getHeight() - mouse_y;
        return vec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
    }

    void Input::init(Window& window)
    {
        this->window_handle = window.getHandle();
        glfwSetScrollCallback(window.getHandle(), scrollCallback);
        glfwSetKeyCallback(window.getHandle(), keyCallback);
        glfwSetCharCallback(window.getHandle(), charCallback);
        glfwSetMouseButtonCallback(window.getHandle(), mouseButtonCallback);
        for (int i = 0; i < 348; ++i)
        {
            down[i] = false;
            released[i] = false;
            repeat[i] = false;
            pressed[i] = false;
        }
    }

    void Input::release()
    {
    }

    float Input::getMouseWheelInput()
    {
        return wheel_offset;
    }

    void Input::setCursorVisible(bool visible)
    {
        visible
            ? glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL)
            : glfwSetInputMode(window_handle, GLFW_CURSOR,
                               GLFW_CURSOR_HIDDEN);
    }

    void Input::setCursorPosition(float x, float y)
    {
        glfwSetCursorPos(window_handle, static_cast<double>(x), static_cast<float>(y));
    }

    void Input::scrollCallback(GLFWwindow* window, double x_offset, double y_offset)
    {
        Input* input = Application::instance->getInput();
        input->wheel_offset = y_offset;
    }

    void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Input* input = Application::instance->getInput();
        input->released[key] = false;
        input->down[key] = false;
        input->repeat[key] = false;
        if (action == GLFW_PRESS)
        {
            input->down[key] = true;
            input->pressed[key] = true;
            input->onKeyDown.invoke(static_cast<KEY>(key));
            input->reset_queue.emplace(key);
        }
        if (action == GLFW_REPEAT)
        {
            input->repeat[key] = true;
        }
        if (action == GLFW_RELEASE)
        {
            input->repeat[key] = false;
            input->pressed[key] = false;
            input->released[key] = true;
            input->reset_queue.emplace(key);
        }
    }

    void Input::mouseButtonCallback(GLFWwindow* window, int key, int action, int mods)
    {
        Input* input = Application::instance->getInput();
        input->released[key] = false;
        input->down[key] = false;
        input->repeat[key] = false;
        if (action == GLFW_PRESS)
        {
            input->down[key] = true;
            input->pressed[key] = true;
            input->reset_queue.emplace(key);
            if (key == GLFW_MOUSE_BUTTON_LEFT)
                input->onMouseLeftClickDown.invoke(input->getMousePosition());
            if (key == GLFW_MOUSE_BUTTON_RIGHT)
                input->onMouseRightClickDown.invoke(input->getMousePosition());
        }
        if (action == GLFW_REPEAT)
        {
            input->repeat[key] = true;
            if (key == GLFW_MOUSE_BUTTON_LEFT)
                input->onMouseLeftClick.invoke(input->getMousePosition());
            if (key == GLFW_MOUSE_BUTTON_RIGHT)
                input->onMouseRightClick.invoke(input->getMousePosition());
        }
        if (action == GLFW_RELEASE)
        {
            input->repeat[key] = false;
            input->pressed[key] = false;
            input->released[key] = true;
            input->reset_queue.emplace(key);

            if (key == GLFW_MOUSE_BUTTON_LEFT)
                input->onMouseLeftClickUp.invoke(input->getMousePosition());
            if (key == GLFW_MOUSE_BUTTON_RIGHT)
                input->onMouseRightClickUp.invoke(input->getMousePosition());
        }
    }

    void Input::pollEvents()
    {
        while (!reset_queue.empty())
        {
            released[reset_queue.front()] = false;
            down[reset_queue.front()] = false;
            reset_queue.pop();
        }
        wheel_offset = 0;
        glfwPollEvents();
    }

    void Input::charCallback(GLFWwindow* window, unsigned int codepoint)
    {
        Input* input = Application::instance->getInput();
        input->onCharDown.invoke(static_cast<char>(codepoint));
    }

    vec2 Input::getNormalizedMousePosition()
    {
        double mouse_x, mouse_y;

        glfwGetCursorPos(window_handle, &mouse_x, &mouse_y);
        return {static_cast<float>(mouse_x / Application::instance->getWindow()->getWidth()), static_cast<float>(mouse_y / Application::instance->getWindow()->getHeight())};
    }
}
