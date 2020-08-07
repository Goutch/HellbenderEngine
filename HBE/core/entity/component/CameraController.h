#pragma once

#include "Component.h"
#define _USE_MATH_DEFINES
#include <math.h>
class Camera;
class CameraController: public Component {
    Camera* camera;
    float current_pitch=0.0f;
    float max_pitch=M_PI/2;
    float units_per_seconds=10.0f;

    vec2 direction=vec2(1);
    double last_x=0,last_y=0;
    void onUpdate(float delta) override;
    void onAttach() override;
    void onDetach() override;
};

