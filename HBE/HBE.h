
#pragma once
#include <CompilationConfigs.h>
#include <core/resource/Resource.h>
#include <core/utility/Log.h>
#include <core/utility/Geometry.h>
#include <core/entity/Entity.h>
#include <core/entity/Scene.h>
#include <core/entity/Transform.h>
#include <core/entity/component/Component.h>
#include <core/entity/component/MeshRenderer.h>
#include <core/entity/component/InstancesRenderer.h>
#include <core/entity/component/Camera.h>
#include <core/entity/component/CameraController.h>
#include <core/graphics/Graphics.h>
#include <core/graphics/RenderTarget.h>
#include <core/input/Input.h>
#include <core/math/Plane.h>

#include <core/utility/Event.h>
#include <core/utility/Clock.h>
#include <core/utility/Job.h>

#include <glm/glm.hpp>
using namespace glm;
class GLFWwindow;

class Scene;

class Clock;

class HBE {
    static GLFWwindow *window;
    static Clock *time;
    static int fps_counter;
    static float fps_timer;
public:
    static Scene *current_scene;

    static void init();

    static void run();

    static Scene *setScene(std::string path);

    static void quit();

    static void terminate();

    static float getTime();
private:
    static void printFPS(float);
};

