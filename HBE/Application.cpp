#include "Application.h"
#include "core/graphics/Graphics.h"
#include "core/input/Input.h"
#include "core/entity/component/Camera.h"
#include "GLFW/glfw3.h"
#include "core/utility/Clock.h"
#include "Configs.h"
#include "core/utility/JobManager.h"
#include <core/serialization/ComponentRegistry.h>
#include <core/entity/component/MeshRenderer.h>
#include <core/entity/component/ModelRenderer.h>
#include <core/entity/component/CameraController.h>
#include <core/entity/component/InstancesRenderer.h>
namespace HBE {
    Scene *Application::scene = nullptr;
    GLFWwindow *Application::window = nullptr;
    Clock *Application::time = nullptr;
    int Application::fps_counter = 0;
    float Application::fps_timer = 0;
    Event<> Application::onInit;
    Event<float> Application::onUpdate;
    Event<Scene *> Application::onSceneChange;
    Event<> Application::onRender;
    Event<> Application::onRegisterComponents;
    void Application::registerComponents() {
        ComponentRegistry::registerComponent<Camera>("Camera");
        ComponentRegistry::registerComponent<MeshRenderer>("MeshRenderer");
        ComponentRegistry::registerComponent<ModelRenderer>("ModelRenderer");
        ComponentRegistry::registerComponent<InstancesRenderer>("InstancesRenderer");
        ComponentRegistry::registerComponent<CameraController>("CameraController");
        onRegisterComponents.invoke();
    }

    void Application::init() {
        window = Graphics::init();
        registerComponents();
        Input::init(window);
        scene = new Scene();
        scene->init();
        onInit.invoke();
    }

    Scene *Application::setScene(std::string path) {
        //todo:scene loading
        if (scene != nullptr) {
            scene->terminate();
            delete scene;
        }
        scene = new Scene();
        scene->init();
        onSceneChange.invoke(scene);
        return scene;
    }

    void Application::run() {
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
            if(Camera::main)
            {
                Graphics::render(Graphics::getRenderTarget(), Camera::main->getProjectionMatrix(), Camera::main->getViewMatrix());
            }
            if (!Configs::isCustonRenderingOn())
                Graphics::present(Graphics::getRenderTarget());
            onRender.invoke();
            Graphics::clearDrawCache();

            delta_t = update_clock.ns() / SECONDS_TO_NANOSECOND;
            update_clock.reset();
#ifdef DEBUG_MODE
            printFPS(delta_t);
#endif
        }
        delete time;
        delete scene;
    }

    void Application::terminate() {
        Graphics::terminate();
        ComponentRegistry::terminate();
    }

    void Application::quit() {
        glfwSetWindowShouldClose(window, true);
    }

    float Application::getTime() {
        if (time) {
            return time->ms() / SECOND_TO_MILISECOND;
        }
        return 0;
    }

    void Application::printFPS(float delta) {
        fps_counter++;
        fps_timer += delta;
        if (fps_timer >= 1.0) {
            Log::debug("fps:" + std::to_string(fps_counter));
            fps_counter = 0;
            fps_timer = 0;
        }
    }
}


