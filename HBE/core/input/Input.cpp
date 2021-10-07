

#include <core/utility/Log.h>
#include "Input.h"
#include "GLFW/glfw3.h"
#include "core/graphics/Graphics.h"
#include "core/graphics/Window.h"

namespace HBE {
	GLFWwindow *Input::window = nullptr;
	bool Input::repeat[348];
	bool Input::pressed[348];
	bool Input::released[348];
	bool Input::down[348];
	float Input::wheel_offset = 0.0f;
	std::queue<short> Input::reset_queue;

	bool Input::getKey(KEY code) {
		return pressed[code];
	}

	bool Input::getKeyRepeat(KEY code) {
		return repeat[code];
	}

	bool Input::getKeyDown(KEY code) {
		return down[code];
	}

	bool Input::getKeyUp(KEY code) {
		return released[code];
	}

	void Input::getMousePosition(double &x, double &y) {
		glfwGetCursorPos(window, &x, &y);
	}

	void Input::init() {
		Input::window = static_cast<GLFWwindow *>(Graphics::getWindow()->getHandle());
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetKeyCallback(window, keyCallback);
		for (int i = 0; i < 348; ++i) {
			down[i] = false;
			released[i] = false;
			repeat[i] = false;
			pressed[i] = false;
		}
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

	void Input::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		released[key] = false;
		down[key] = false;
		repeat[key] = false;
		if (action == GLFW_PRESS) {
			down[key] = true;
			pressed[key] = true;
			reset_queue.emplace(key);
		}
		if (action == GLFW_REPEAT) {
			repeat[key] = true;
		}
		if (action == GLFW_RELEASE) {
			repeat[key] = false;
			pressed[key] = false;
			released[key] = true;
			reset_queue.emplace(key);
		}
	}

	void Input::pollEvents() {
		while (!reset_queue.empty()) {
			released[reset_queue.front()] = false;
			down[reset_queue.front()] = false;
			reset_queue.pop();
		}
		glfwPollEvents();
	}


}




