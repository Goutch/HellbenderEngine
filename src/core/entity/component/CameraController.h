#pragma once

#include "Component.h"
class Camera;
class CameraController: public Component {
    Camera* camera;
    float current_pitch=0;
    vec2 direction=vec2(1);
    double last_x=0,last_y=0;
    void onUpdate(float delta) override;
    void onAttach() override;
    void onDetach() override;
};

