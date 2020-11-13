#include "HBE.h"
#include "GLFW/glfw3.h"
#include "core/scripting/ScriptManager.h"

Scene *HBE::scene = nullptr;
GLFWwindow *HBE::window = nullptr;
Clock *HBE::time = nullptr;
int HBE::fps_counter = 0;
float HBE::fps_timer = 0;
Event<> HBE::onInit;
Event<float> HBE::onUpdate;
Event<Scene *> HBE::onSceneChange;
Event<> HBE::onRender;

void HBE::init() {
    window = Graphics::init();
    Input::init(window);
    scene = new Scene();
    scene->init();
    ScriptManager::init();
    onInit.invoke();
}

Scene *HBE::setScene(std::string path) {
    if (scene != nullptr) {
        scene->terminate();
        delete scene;
    }
    scene = new Scene();
    scene->init();
    onSceneChange.invoke(scene);
    return scene;
}

void HBE::run() {
    time = new Clock();
    Clock update_clock = Clock();
    float delta_t = 0.0f;
    if (Camera::main == nullptr) {
        scene->instantiate<Camera>();
    }
    while (!glfwWindowShouldClose(window) && scene != nullptr) {
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (Input::getKeyDown(KEY::ESCAPE))
            quit();
        JobManager::updateJobsStatus();
        scene->update(delta_t);
        onUpdate.invoke(delta_t);
        scene->draw();
        Graphics::render(Graphics::getRenderTarget(),Camera::main->getProjectionMatrix(), Camera::main->getViewMatrix());
        if (!Configs::getCustomRendering())
            Graphics::present(Graphics::getRenderTarget());
        onRender.invoke();
        Graphics::clearDrawCache();

        delta_t = update_clock.ns() / SECONDS_TO_NANOSECOND;
        update_clock.reset();
#if DEBUG_MODE
        printFPS(delta_t);
#endif
    }
    delete time;
    delete scene;
}

void HBE::terminate() {
    ScriptManager::terminate();
    Graphics::terminate();
}

void HBE::quit() {
    glfwSetWindowShouldClose(window, true);
}

float HBE::getTime() {
    if (time) {
        return time->ms() / SECOND_TO_MILISECOND;
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


