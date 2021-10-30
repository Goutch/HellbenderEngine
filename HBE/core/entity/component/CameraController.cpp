#include "CameraController.h"
#include "core/input/Input.h"
#include "Camera.h"
#include "HBE/core/resource/RenderTarget.h"
#include "core/serialization/Serializer.h"
#include "core/entity/Entity.h"
#include "core/graphics/Graphics.h"
#include "core/graphics/Window.h"
namespace HBE {
    void CameraController::onUpdate(float delta) {
        uint32_t w, h;
        camera->getRenderTarget()->getResolution(w,h);

        double x, y;
        Input::getMousePosition(x, y);
        Input::setCursorPosition(w / 2.0, h / 2.0f);

        float fov = camera->getFOV();
        vec2 change;
        change.x = ((((float)w / 2.0f) - x) / (float)w) * glm::radians(fov);
        change.y = ((((float)h / 2.0f) - y) / (float)h) * (glm::radians(fov) * camera->getAspectRatio());

        //go to pitch 0
        entity->transform->rotate(quat(vec3(-current_pitch, 0, 0)));
        //rotate on y axis
        entity->transform->rotate(quat(vec3(0, change.x, 0)));
        //go back to current pitch
        current_pitch += change.y;
        current_pitch = glm::clamp(current_pitch, -MAX_PITCH, MAX_PITCH);

        entity->transform->rotate(current_pitch, vec3(1, 0, 0));


        vec3 translation = vec3(0);
        if (Input::getKey(KEY::S)) {
            translation.z += 1;
        }
        if (Input::getKey(KEY::W)) {
            translation.z = -1;
        }
        if (Input::getKey(KEY::D)) {
            translation.x += 1;
        }
        if (Input::getKey(KEY::A)) {
            translation.x = -1;
        }
        if (Input::getKey(KEY::SPACE)) {
            translation.y += 1;
        }
        if (Input::getKey(KEY::LEFT_CONTROL)) {
            translation.y = -1;
        }

        entity->transform->translate(translation * delta * units_per_seconds);

    }

    void CameraController::onAttach() {
        Component::onAttach();
        camera = entity->getComponent<Camera>();
        if (camera == nullptr) {
            camera = entity->attach<Camera>();
        }
        Input::setCursorVisible(false);
        subscribeUpdate();
    }

    CameraController::~CameraController() {
        Input::setCursorVisible(true);
    }

    std::string CameraController::toString() const {
        return "CameraController";
    }

}

