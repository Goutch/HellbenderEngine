#include "HBE.h"
#include <core/graphics/Graphics.h>
#include <core/entity/Scene.h>
#include <core/entity/component/Camera.h>
#include <core/input/Input.h>
#include "GLFW/glfw3.h"
#include "core/utility/Timer.h"

Scene *HBE::current_scene = nullptr;
GLFWwindow *HBE::window = nullptr;
Timer *HBE::time = nullptr;

void HBE::init() {
    window = Graphics::init();
    Input::init(window);
    current_scene=new Scene();
    current_scene->init();
}

Scene *HBE::setScene(std::string path) {
    if (current_scene != nullptr) {
        current_scene->terminate();
        delete current_scene;
    }
    current_scene = new Scene();
    current_scene->init();
    return current_scene;
}

void HBE::run() {
    time = new Timer();
    Timer delta = Timer();
    float delta_t = 0.0f;
    if(Camera::main== nullptr)
    {
        current_scene->instantiate<Camera>();
    }
    while (!glfwWindowShouldClose(window) && current_scene != nullptr) {
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE))quit();
        current_scene->update(delta_t);
        current_scene->draw();
        Graphics::render(Camera::main->getProjectionMatrix(), Camera::main->getViewMatrix());
        delta_t = delta.ms();
        delta.reset();
    }
    delete time;
    delete current_scene;
}

void HBE::terminate() {
    Graphics::terminate();
    glfwTerminate();
}

void HBE::quit() {
    glfwSetWindowShouldClose(window, true);
}

float HBE::getTime() {
    return time->ms();
}



