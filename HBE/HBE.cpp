#include "HBE.h"
#include "GLFW/glfw3.h"
#include "scripting/ScriptManager.h"

Scene *HBE::current_scene = nullptr;
GLFWwindow *HBE::window = nullptr;
Clock *HBE::time = nullptr;
int HBE::fps_counter = 0;
float HBE::fps_timer = 0;
Event<> HBE::onInit;
Event<float> HBE::onUpdate;
Event<Scene*> HBE::onSceneChange;
Event<RenderTarget*> HBE::onRenderFinish;
void HBE::init() {
    window = Graphics::init();
    Input::init(window);
    current_scene = new Scene();
    current_scene->init();
    ScriptManager::init();
    onInit.invoke();
}

Scene *HBE::setScene(std::string path) {
    if (current_scene != nullptr) {
        current_scene->terminate();
        delete current_scene;
    }
    current_scene = new Scene();
    current_scene->init();
    onSceneChange.invoke(current_scene);
    return current_scene;
}

void HBE::run() {
    time = new Clock();
    Clock update_clock = Clock();
    float delta_t = 0.0f;
    if (Camera::main == nullptr) {
        current_scene->instantiate<Camera>();
    }
    while (!glfwWindowShouldClose(window) && current_scene != nullptr) {
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (Input::getKeyDown(KEY::ESCAPE))
            quit();
        JobManager::updateJobsStatus();
        current_scene->update(delta_t);
        onUpdate.invoke(delta_t);
        current_scene->draw();
        Graphics::render(Camera::main->getProjectionMatrix(), Camera::main->getViewMatrix());
        onRenderFinish.invoke(Graphics::getRenderTarget());
        delta_t = update_clock.ns() / SECONDS_TO_NANOSECOND;
        update_clock.reset();
#if DEBUG_MODE
        printFPS(delta_t);
#endif
    }
    delete time;
    delete current_scene;
}

void HBE::terminate() {
    ScriptManager::terminate();
    Graphics::terminate();
}

void HBE::quit() {
    glfwSetWindowShouldClose(window, true);
}

float HBE::getTime() {
    if(time)
    {
        return time->ms()/SECOND_TO_MILISECOND;
    }
    return 0;
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


