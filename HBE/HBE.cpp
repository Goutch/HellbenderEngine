#include "HBE.h"
#include "GLFW/glfw3.h"

Scene *HBE::current_scene = nullptr;
GLFWwindow *HBE::window = nullptr;
Clock *HBE::time = nullptr;
int HBE::fps_counter = 0;
float HBE::fps_timer = 0;

void HBE::init() {
    window = Graphics::init();
    Input::init(window);
    current_scene = new Scene();
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
    time = new Clock();
    Clock delta = Clock();
    float delta_t = 0.0f;
    if (Camera::main == nullptr) {
        current_scene->instantiate<Camera>();
    }
    while (!glfwWindowShouldClose(window) && current_scene != nullptr) {
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE))quit();

        current_scene->update(delta_t);
        current_scene->draw();
        Graphics::render(Camera::main->getProjectionMatrix(), Camera::main->getViewMatrix());
        delta_t = delta.ns()/SECONDS_TO_NANOSECOND;
        delta.reset();
#if DEBUG_MODE
        printFPS(delta_t);
#endif
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

void HBE::printFPS(float delta) {
    fps_counter++;
    fps_timer += delta;
    if (fps_timer >= 1.0) {
        Log::debug("fps:" + std::to_string(fps_counter));
        fps_counter = 0;
        fps_timer = 0;
    }

}


