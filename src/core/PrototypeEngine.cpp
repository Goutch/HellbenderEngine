#include "PrototypeEngine.h"
#include <core/graphics/Graphics.h>
#include <core/entity/Scene.h>
#include <core/entity/component/Camera.h>
#include <core/input/Input.h>
#include "GLFW/glfw3.h"
#include "core/utility/Timer.h"

Scene *PrototypeEngine::current_scene = nullptr;
GLFWwindow *PrototypeEngine::window = nullptr;
Timer *PrototypeEngine::time = nullptr;

void PrototypeEngine::init() {
    window = Graphics::init();
    Input::init(window);
}

Scene *PrototypeEngine::setScene(std::string path) {
    if (current_scene != nullptr) {
        current_scene->terminate();
        delete current_scene;
    }
    current_scene = new Scene();
    current_scene->init();
    return current_scene;
}

void PrototypeEngine::run() {
    if (current_scene == nullptr) {
        current_scene = new Scene();
        current_scene->init();
    }
    time = new Timer();
    Timer delta = Timer();
    float delta_t = 0.0f;
    while (!glfwWindowShouldClose(window) && current_scene != nullptr) {
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE))quit();
        current_scene->update(delta_t);
        current_scene->draw();
        Graphics::render(Camera::main->getProjectionMatrix(), Camera::main->getViewMatrix());
        delta.reset();
        delta_t = delta.ms();
    }
    delete time;
    delete current_scene;
}

void PrototypeEngine::terminate() {
    Graphics::terminate();
    glfwTerminate();
}

void PrototypeEngine::quit() {
    glfwSetWindowShouldClose(window, true);
}

float PrototypeEngine::getTime() {
    return time->ms();
}



