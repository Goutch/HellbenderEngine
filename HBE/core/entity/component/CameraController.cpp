//
// Created by User on 12-Jun.-2020.
//
#include <HBE.h>
#include "CompilationConfigs.h"
#include "CameraController.h"


void CameraController::onUpdate(float delta) {
    double x, y;
    Input::getMousePosition(x, y);
    Input::setCursorPosition(static_cast<float>(WIDTH) / 2.0f, static_cast<float>(HEIGHT) / 2.0f);
    last_x = x;
    last_y = y;

    float fov = camera->getFOV();
    vec2 change;
    change.x = (((static_cast<float>(WIDTH) / 2.0f) - x) / static_cast<float>(WIDTH)) * glm::radians(fov);
    change.y = (((static_cast<float>(HEIGHT) / 2.0f) - y) / static_cast<float>(HEIGHT)) *
               (glm::radians(fov) * camera->getAspectRatio());

    //go to pitch 0
    entity->rotate(quat(vec3(-current_pitch, 0, 0)));
    //rotate on y axis
    entity->rotate(quat(vec3(0, change.x, 0)));
    //go back to cuurent pitch
    current_pitch += change.y;
    if (current_pitch > max_pitch) {
        current_pitch = max_pitch;
    } else if (current_pitch < -max_pitch) {
        current_pitch = -max_pitch;
    }
    entity->rotate(current_pitch, vec3(1, 0, 0));


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

    entity->translate(translation * delta * units_per_seconds);

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

void CameraController::onDetach() {
    Component::onDetach();
    Input::setCursorVisible(true);
}

void CameraController::serialize(Serializer *serializer) const {
    Component::serialize(serializer);
}
