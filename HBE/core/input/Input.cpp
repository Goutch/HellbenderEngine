

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

	vec2 Input::getMousePosition() {
		double mouse_x, mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		return vec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
	}

	void Input::init() {
		Input::window = static_cast<GLFWwindow *>(Graphics::getWindow()->getHandle());
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetKeyCallback(window, keyCallback);
		glfwSetMouseButtonCallback(window, mouseButtonCallback);
		for (int i = 0; i < 348; ++i) {
			down[i] = false;
			released[i] = false;
			repeat[i] = false;
			pressed[i] = false;
		}
	}

	float Input::getMouseWheelInput() {
		return wheel_offset;
	}

	void Input::setCursorVisible(bool visible) {
		visible ? glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL) : glfwSetInputMode(window, GLFW_CURSOR,
																							   GLFW_CURSOR_HIDDEN);
	}

	void Input::setCursorPosition(float x, float y) {
		glfwSetCursorPos(window, static_cast<double>(x), static_cast<float>(y));
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

	void Input::mouseButtonCallback(GLFWwindow *window, int key, int action, int mods) {
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
		wheel_offset = 0;
		glfwPollEvents();
	}


}




